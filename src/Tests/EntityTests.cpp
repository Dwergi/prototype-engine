#include "PrecompiledHeader.h"
#include "Entity.h"

#include "catch/catch.hpp"

struct FirstComponent
{
	DD_COMPONENT

	int Value;
};

struct SecondComponent
{
	DD_COMPONENT

	int OtherValue;
};

struct ThirdComponent
{
	DD_COMPONENT

	int OtherValue;
};

DD_COMPONENT_CPP( FirstComponent );
DD_COMPONENT_CPP( SecondComponent );
DD_COMPONENT_CPP( ThirdComponent );

struct TestSystem : ddc::System
{
	TestSystem() :
		System( "TestSystem" ),
		s_firstRead( *this ),
		s_secondWrite( *this )
	{

	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		ddc::ReadBuffer<FirstComponent> read = data.GetRead<FirstComponent>();
		ddc::WriteBuffer<SecondComponent> write = data.GetWrite<SecondComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( int i = 0; i < read.Size(); ++i )
		{
			const FirstComponent& cmp = read.Get( i );
			write.Get( i ).OtherValue = cmp.Value;
		}
	}

	ddc::ReadRequirement<FirstComponent> s_firstRead;
	ddc::WriteRequirement<SecondComponent> s_secondWrite;
};

struct DependentSystem : ddc::System
{
	DependentSystem() :
		System( "DependentSystem" ),
		s_secondRead( *this ),
		s_thirdWrite( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override	{}

	ddc::ReadRequirement<SecondComponent> s_secondRead;
	ddc::WriteRequirement<ThirdComponent> s_thirdWrite;
};

struct ReaderSystem : ddc::System
{
	ReaderSystem() :
		System( "ReaderSystem" ),
		s_thirdRead( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override {}

	ddc::ReadRequirement<ThirdComponent> s_thirdRead;
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
		System( "OnlyReaderSystem" ),
		s_firstRead( *this ),
		s_secondRead( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override {}

	ddc::ReadRequirement<FirstComponent> s_firstRead;
	ddc::ReadRequirement<SecondComponent> s_secondRead;
};

struct OnlyWriterSystem : ddc::System
{
	OnlyWriterSystem() :
		System( "OnlyWriterSystem" ),
		s_secondWrite( *this ),
		s_thirdWrite( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override {}

	ddc::WriteRequirement<SecondComponent> s_secondWrite;
	ddc::WriteRequirement<ThirdComponent> s_thirdWrite;
};

TEST_CASE( "EntityManager" )
{
	ddc::EntityLayer layer;

	ddc::Entity a = layer.Create();
	REQUIRE( a == 0 );

	ddc::Entity b = layer.Create();
	REQUIRE( b == 1 );

	ddc::Entity c = layer.Create();
	REQUIRE( c == 2 );

	layer.Destroy( a );
	
	ddc::Entity a2 = layer.Create();
	REQUIRE( a2 == a );

	layer.Destroy( b );

	ddc::Entity b2 = layer.Create();
	REQUIRE( b2 == b );
}

TEST_CASE( "Component" )
{
	ddc::EntityLayer layer;
	ddc::Entity a = layer.Create();

	bool found = layer.HasComponent<FirstComponent>( a );
	REQUIRE( found == false );

	FirstComponent& cmp = layer.AddComponent<FirstComponent>( a );
	REQUIRE( cmp.Value == 0 );

	REQUIRE( &cmp.GetType() == &FirstComponent::Type );

	cmp.Value = 5;

	const FirstComponent& cmp2 = *layer.AccessComponent<FirstComponent>( a );
	REQUIRE( cmp2.Value == 5 );
	REQUIRE( cmp.Value == cmp2.Value );

	REQUIRE( layer.HasComponent<FirstComponent>( a ) == true );

	layer.RemoveComponent<FirstComponent>( a );
	REQUIRE( layer.HasComponent<FirstComponent>( a ) == false );
}

TEST_CASE( "Update System" )
{
	TestSystem system;

	ddc::EntityLayer layer;

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity entity = layer.Create();

		FirstComponent& simple = layer.AddComponent<FirstComponent>( entity );
		simple.Value = i;

		SecondComponent& other = layer.AddComponent<SecondComponent>( entity );
		other.OtherValue = -1;
	}

	ddc::UpdateSystem( layer, system );

	for( int i = 0; i < 4; ++i )
	{
		FirstComponent& simple = *layer.AccessComponent<FirstComponent>( i );
		REQUIRE( simple.Value == i );

		SecondComponent& other = *layer.AccessComponent<SecondComponent>( i );
		REQUIRE( other.OtherValue == i );
	}

	for( int i = 0; i < 4; ++i )
	{
		layer.RemoveComponent<FirstComponent>( i );
		REQUIRE_FALSE( layer.HasComponent<FirstComponent>( i ) );

		layer.RemoveComponent<SecondComponent>( i );
		REQUIRE_FALSE( layer.HasComponent<SecondComponent>( i ) );
	}
}

TEST_CASE( "Schedule Systems By Component" )
{
	SECTION( "Simple" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;

		ddc::System* systems[] = { &dependent_system, &test_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &dependent_system );
	}

	SECTION( "Independent" )
	{
		TestSystem test_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &reader_system );
	}

	SECTION( "Dependency Chain" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &dependent_system, &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &dependent_system );
		REQUIRE( ordered[ 2 ] == &reader_system );
	}

	SECTION( "Duplicate Requirements" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;
		DependentSystem duplicate_system;

		ddc::System* systems[] = { &dependent_system, &test_system, &duplicate_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
	}

	SECTION( "Only Writer" )
	{
		OnlyWriterSystem only_writer_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &only_writer_system, &dependent_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &only_writer_system );
		REQUIRE( ordered[ 1 ] == &dependent_system );
		REQUIRE( ordered[ 2 ] == &reader_system );
	}

	SECTION( "Only Reader" )
	{
		TestSystem test_system;
		OnlyReaderSystem only_reader_system;

		ddc::System* systems[] = { &test_system, &only_reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &only_reader_system );
	}
}

TEST_CASE( "Schedule Systems By Dependency" )
{
	SECTION( "Simple" )
	{
		TestSystem a;

		TestSystem b;
		b.RegisterDependency( a );

		ddc::System* systems[] = { &a, &b };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &a );
		REQUIRE( ordered[ 1 ] == &b );
	}

	SECTION( "Chain" )
	{
		TestSystem a;

		TestSystem b;
		b.RegisterDependency( a );

		TestSystem c;
		c.RegisterDependency( b );

		ddc::System* systems[] = { &a, &b, &c };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &a );
		REQUIRE( ordered[ 1 ] == &b );
		REQUIRE( ordered[ 2 ] == &c );
	}

	SECTION( "Diamond" )
	{
		TestSystem a;

		TestSystem b;
		b.RegisterDependency( a );

		TestSystem c;
		c.RegisterDependency( a );

		TestSystem d;
		d.RegisterDependency( b );
		d.RegisterDependency( c );

		ddc::System* systems[] = { &a, &b, &c, &d };

		dd::Span<ddc::System*> span_systems( systems, 4 );

		std::vector<ddc::System*> ordered;
		ddc::ScheduleSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &a );
		REQUIRE( ordered[ 1 ] == &b );
		REQUIRE( ordered[ 2 ] == &c );
		REQUIRE( ordered[ 3 ] == &d );
	}
}