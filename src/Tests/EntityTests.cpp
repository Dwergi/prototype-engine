#include "PrecompiledHeader.h"
#include "EntityLayer.h"

#include "ComponentDataBuffer.h"
#include "JobSystem.h"
#include "UpdateData.h"
#include "System.h"

#include "catch2/catch.hpp"

struct FirstComponent
{
	DD_COMPONENT;

	int FirstValue { -100 };
};

struct SecondComponent
{
	DD_COMPONENT;

	int SecondValue { 0 };
};

struct ThirdComponent
{
	DD_COMPONENT;

	int ThirdValue { 0 };
};

DD_COMPONENT_CPP( FirstComponent );
DD_COMPONENT_CPP( SecondComponent );
DD_COMPONENT_CPP( ThirdComponent );

struct TestSystem : ddc::System
{
	TestSystem() :
		System( "TestSystem" ),
		m_firstRead( *this ),
		m_secondWrite( *this )
	{

	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		ddc::ReadBuffer<FirstComponent> read = data.GetRead<FirstComponent>();
		ddc::WriteBuffer<SecondComponent> write = data.GetWrite<SecondComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const FirstComponent& cmp = read.Get( i );
			write.Get( i ).SecondValue = cmp.FirstValue;
		}
	}

	ddc::ReadRequirement<FirstComponent> m_firstRead;
	ddc::WriteRequirement<SecondComponent> m_secondWrite;
};

struct DependentSystem : ddc::System
{
	DependentSystem() :
		System( "DependentSystem" ),
		m_secondRead( *this ),
		m_thirdWrite( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		ddc::ReadBuffer<SecondComponent> read = data.GetRead<SecondComponent>();
		ddc::WriteBuffer<ThirdComponent> write = data.GetWrite<ThirdComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const SecondComponent& cmp = read.Get( i );
			write.Get( i ).ThirdValue = cmp.SecondValue;
		}
	}

	ddc::ReadRequirement<SecondComponent> m_secondRead;
	ddc::WriteRequirement<ThirdComponent> m_thirdWrite;
};

struct ReaderSystem : ddc::System
{
	ReaderSystem() :
		System( "ReaderSystem" ),
		m_thirdRead( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		ddc::ReadBuffer<ThirdComponent> read = data.GetRead<ThirdComponent>();

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const ThirdComponent& cmp = read.Get( i );
			int x = cmp.ThirdValue * cmp.ThirdValue;
		}
	}

	ddc::ReadRequirement<ThirdComponent> m_thirdRead;
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
		System( "OnlyReaderSystem" ),
		m_firstRead( *this ),
		m_secondRead( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		ddc::ReadBuffer<FirstComponent> read1 = data.GetRead<FirstComponent>();
		ddc::ReadBuffer<SecondComponent> read2 = data.GetRead<SecondComponent>();

		for( size_t i = 0; i < read1.Size(); ++i )
		{
			const FirstComponent& cmp1 = read1.Get( i );
			const SecondComponent& cmp2 = read2.Get( i );
			int x = cmp1.FirstValue * cmp2.SecondValue;
		}
	}

	ddc::ReadRequirement<FirstComponent> m_firstRead;
	ddc::ReadRequirement<SecondComponent> m_secondRead;
};

struct OnlyWriterSystem : ddc::System
{
	OnlyWriterSystem() :
		System( "OnlyWriterSystem" ),
		m_secondWrite( *this ),
		m_thirdWrite( *this )
	{
	}

	virtual void Update( const ddc::UpdateData& data ) override {}

	ddc::WriteRequirement<SecondComponent> m_secondWrite;
	ddc::WriteRequirement<ThirdComponent> m_thirdWrite;
};

TEST_CASE( "EntityManager" )
{
	ddc::EntityLayer layer;

	ddc::Entity a = layer.Create();
	REQUIRE( a.ID == 0 );

	ddc::Entity b = layer.Create();
	REQUIRE( b.ID == 1 );

	ddc::Entity c = layer.Create();
	REQUIRE( c.ID == 2 );

	layer.Destroy( a );
	
	ddc::Entity a2 = layer.Create();
	REQUIRE( a2.ID == a.ID );
	REQUIRE( a2.Version == 1 );

	layer.Destroy( b );

	ddc::Entity b2 = layer.Create();
	REQUIRE( b2.ID == b.ID );
	REQUIRE( b2.Version == 1 );
}

TEST_CASE( "Component" )
{
	ddc::EntityLayer layer;
	ddc::Entity a = layer.Create();

	bool found = layer.HasComponent<FirstComponent>( a );
	REQUIRE( found == false );

	FirstComponent& cmp = layer.AddComponent<FirstComponent>( a );
	REQUIRE( cmp.FirstValue == -100 );

	REQUIRE( &cmp.GetType() == &FirstComponent::Type );

	cmp.FirstValue = 5;

	const FirstComponent& cmp2 = *layer.AccessComponent<FirstComponent>( a );
	REQUIRE( cmp2.FirstValue == 5 );
	REQUIRE( cmp.FirstValue == cmp2.FirstValue );

	REQUIRE( layer.HasComponent<FirstComponent>( a ) );

	layer.RemoveComponent<FirstComponent>( a );
	REQUIRE_FALSE( layer.HasComponent<FirstComponent>( a ) );
}

TEST_CASE( "Update System" )
{
	ddc::EntityLayer layer;

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e = layer.Create();

		FirstComponent& simple = layer.AddComponent<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = layer.AddComponent<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	ddc::UpdateSystem( system, layer );

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *layer.AccessComponent<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *layer.AccessComponent<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update With Discontinuity" )
{
	ddc::EntityLayer layer;

	for( int i = 0; i < 5; ++i )
	{
		ddc::Entity e = layer.Create();

		if( i == 2 )
			continue;
		
		FirstComponent& simple = layer.AddComponent<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = layer.AddComponent<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	ddc::UpdateSystem( system, layer, 1 );

	for( int i = 0; i < 5; ++i )
	{
		if( i == 2 )
			continue;

		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *layer.AccessComponent<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *layer.AccessComponent<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update Multiple Systems" )
{
	TestSystem a;
	DependentSystem b;

	ddc::EntityLayer layer;

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e = layer.Create();

		FirstComponent& first = layer.AddComponent<FirstComponent>( e );
		first.FirstValue = i;

		SecondComponent& second = layer.AddComponent<SecondComponent>( e );
		second.SecondValue = -1;

		ThirdComponent& third = layer.AddComponent<ThirdComponent>( e );
		third.ThirdValue = -1;
	}

	ddc::UpdateSystem( a, layer );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		SecondComponent& second = *layer.AccessComponent<SecondComponent>( e );
		REQUIRE( second.SecondValue == e.ID );
	}

	ddc::UpdateSystem( b, layer );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		ThirdComponent& third = *layer.AccessComponent<ThirdComponent>( e );
		REQUIRE( third.ThirdValue == e.ID );
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

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &test_system );
		REQUIRE( ordered[ 1 ].m_system == &dependent_system );
	}

	SECTION( "Independent" )
	{
		TestSystem test_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &test_system );
		REQUIRE( ordered[ 1 ].m_system == &reader_system );
	}

	SECTION( "Dependency Chain" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &dependent_system, &test_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &test_system );
		REQUIRE( ordered[ 1 ].m_system == &dependent_system );
		REQUIRE( ordered[ 2 ].m_system == &reader_system );
	}

	SECTION( "Duplicate Requirements" )
	{
		TestSystem test_system;
		DependentSystem dependent_system;
		DependentSystem duplicate_system;

		ddc::System* systems[] = { &dependent_system, &test_system, &duplicate_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &test_system );
	}

	SECTION( "Only Writer" )
	{
		OnlyWriterSystem only_writer_system;
		DependentSystem dependent_system;
		ReaderSystem reader_system;

		ddc::System* systems[] = { &only_writer_system, &dependent_system, &reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &only_writer_system );
		REQUIRE( ordered[ 1 ].m_system == &dependent_system );
		REQUIRE( ordered[ 2 ].m_system == &reader_system );
	}

	SECTION( "Only Reader" )
	{
		TestSystem test_system;
		OnlyReaderSystem only_reader_system;

		ddc::System* systems[] = { &test_system, &only_reader_system };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByComponent( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &test_system );
		REQUIRE( ordered[ 1 ].m_system == &only_reader_system );
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

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
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

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );
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

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );
		REQUIRE( ordered[ 3 ].m_system == &d );
	}
}

TEST_CASE( "Update With Tree Scheduling" )
{
	SECTION( "Simple" )
	{
		TestSystem a;

		TestSystem b;
		b.RegisterDependency( a );

		ddc::System* systems[] = { &a, &b };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );

		dd::JobSystem jobsystem( 0u );
		ddc::EntityLayer layer;

		ddc::UpdateSystemsWithTreeScheduling( ordered, jobsystem, layer );
	}

	SECTION( "Multiple Roots" )
	{
		TestSystem a;
		TestSystem b;

		TestSystem c;
		c.RegisterDependency( a );
		c.RegisterDependency( b );

		ddc::System* systems[] = { &a, &b, &c };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );

		dd::JobSystem jobsystem( 0u );
		ddc::EntityLayer layer;

		ddc::UpdateSystemsWithTreeScheduling( ordered, jobsystem, layer );
	}

	SECTION( "Diamond" )
	{
		TestSystem a;

		TestSystem b;
		TestSystem c;
		c.RegisterDependency( a );
		c.RegisterDependency( b );

		TestSystem d;
		d.RegisterDependency( c );

		ddc::System* systems[] = { &a, &b, &c, &d };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );
		REQUIRE( ordered[ 3 ].m_system == &d );

		dd::JobSystem jobsystem( 0u );
		ddc::EntityLayer layer;

		ddc::UpdateSystemsWithTreeScheduling( ordered, jobsystem, layer );
	}
}

TEST_CASE( "Full Update Loop" )
{
	TestSystem a;
	DependentSystem b;
	ReaderSystem c;

	ddc::System* systems[] = { &a, &b, &c };
	ddc::EntityLayer layer;

	BENCHMARK( "Create Entities" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			ddc::Entity e = layer.Create();
			FirstComponent& first = layer.AddComponent<FirstComponent>( e );
			first.FirstValue = i;

			SecondComponent& second = layer.AddComponent<SecondComponent>( e );
			second.SecondValue = 0;

			ThirdComponent& third = layer.AddComponent<ThirdComponent>( e );
			third.ThirdValue = 0;
		}
	}

	BENCHMARK( "Update" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			ddc::UpdateSystem( a, layer );
			ddc::UpdateSystem( b, layer );
			ddc::UpdateSystem( c, layer );
		}
	}

	for( int i = 0; i < 1000; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		const SecondComponent* second = layer.GetComponent<SecondComponent>( e );
		REQUIRE( second->SecondValue == i );

		const ThirdComponent* third = layer.GetComponent<ThirdComponent>( e );
		REQUIRE( third->ThirdValue == i );
	}
}