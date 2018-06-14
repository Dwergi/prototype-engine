#include "PrecompiledHeader.h"
#include "Entity.h"

#include "catch/catch.hpp"

struct ThirdComponent
{
	COMPONENT_H();

	int OtherValue;
};

struct FirstComponent
{
	COMPONENT_H();

	int Value;
};

struct SecondComponent
{
	COMPONENT_H();

	int OtherValue;
};

COMPONENT_CPP( ThirdComponent );
COMPONENT_CPP( FirstComponent );
COMPONENT_CPP( SecondComponent );

struct TestSystem : ddc::System
{
	TestSystem() :
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
		s_thirdRead( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override {}

	ddc::ReadRequirement<ThirdComponent> s_thirdRead;
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
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
	ddc::EntitySpace mgr;

	ddc::Entity a = mgr.Create();
	REQUIRE( a == 0 );

	ddc::Entity b = mgr.Create();
	REQUIRE( b == 1 );

	ddc::Entity c = mgr.Create();
	REQUIRE( c == 2 );

	mgr.Destroy( a );
	
	ddc::Entity a2 = mgr.Create();
	REQUIRE( a2 == a );

	mgr.Destroy( b );

	ddc::Entity b2 = mgr.Create();
	REQUIRE( b2 == b );
}

TEST_CASE( "Component" )
{
	ddc::EntitySpace mgr;
	ddc::Entity a = mgr.Create();

	bool found = mgr.HasComponent<FirstComponent>( a );
	REQUIRE( found == false );

	FirstComponent& cmp = *mgr.AddComponent<FirstComponent>( a );
	REQUIRE( cmp.Value == 0 );

	cmp.Value = 5;

	const FirstComponent& cmp2 = *mgr.AccessComponent<FirstComponent>( a );
	REQUIRE( cmp2.Value == 5 );
	REQUIRE( cmp.Value == cmp2.Value );

	REQUIRE( mgr.HasComponent<FirstComponent>( a ) == true );

	mgr.RemoveComponent<FirstComponent>( a );
	REQUIRE( mgr.HasComponent<FirstComponent>( a ) == false );
}

TEST_CASE( "Update System" )
{
	TestSystem system;

	ddc::EntitySpace space;

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity entity = space.Create();

		FirstComponent& simple = *space.AddComponent<FirstComponent>( entity );
		simple.Value = i;

		SecondComponent& other = *space.AddComponent<SecondComponent>( entity );
		other.OtherValue = -1;
	}

	ddc::UpdateSystem( space, system );

	for( int i = 0; i < 4; ++i )
	{
		FirstComponent& simple = *space.AccessComponent<FirstComponent>( i );
		REQUIRE( simple.Value == i );

		SecondComponent& other = *space.AccessComponent<SecondComponent>( i );
		REQUIRE( other.OtherValue == i );
	}
}

TEST_CASE( "Schedule Systems" )
{
	SECTION( "Simple" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;

		ddc::System* systems[] = { &dependent_system, &test_system };

		dd::Span<ddc::System*> span_systems( systems, 2 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &dependent_system );
	}

	SECTION( "Independent" )
	{
		TestSystem test_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems, 2 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &reader_system );
	}

	SECTION( "Dependency Chain" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &dependent_system, &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems, 3 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

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

		dd::Span<ddc::System*> span_systems( systems, 3 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
	}

	SECTION( "Only Writer" )
	{
		OnlyWriterSystem only_writer_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &only_writer_system, &dependent_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems, 3 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &only_writer_system );
		REQUIRE( ordered[ 1 ] == &dependent_system );
		REQUIRE( ordered[ 2 ] == &reader_system );
	}

	SECTION( "Only Reader" )
	{
		TestSystem test_system;
		OnlyReaderSystem only_reader_system;

		ddc::System* systems[] = { &test_system, &only_reader_system };

		dd::Span<ddc::System*> span_systems( systems, 2 );

		std::vector<ddc::System*> ordered;
		ScheduleSystems( span_systems, ordered );

		REQUIRE( ordered[ 0 ] == &test_system );
		REQUIRE( ordered[ 1 ] == &only_reader_system );
	}
}
