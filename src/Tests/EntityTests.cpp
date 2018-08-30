#include "PrecompiledHeader.h"
#include "World.h"

#include "ComponentBuffer.h"
#include "JobSystem.h"
#include "UpdateData.h"
#include "System.h"
#include "SystemsSorting.h"

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
		System( "TestSystem" )
	{
		RequireRead<FirstComponent>();
		RequireWrite<SecondComponent>();
	}

	virtual void Update( const ddc::UpdateData& data, float delta_t ) override
	{
		ddc::ReadBuffer<FirstComponent> read = data.Read<FirstComponent>();
		ddc::WriteBuffer<SecondComponent> write = data.Write<SecondComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const FirstComponent& cmp = read.Get( i );
			write.Access( i ).SecondValue = cmp.FirstValue;
		}
	}
};

struct DependentSystem : ddc::System
{
	DependentSystem() :
		System( "DependentSystem" )
	{
		RequireRead<SecondComponent>();
		RequireWrite<ThirdComponent>();
	}

	virtual void Update( const ddc::UpdateData& data, float delta_t ) override
	{
		ddc::ReadBuffer<SecondComponent> read = data.Read<SecondComponent>();
		ddc::WriteBuffer<ThirdComponent> write = data.Write<ThirdComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const SecondComponent& cmp = read.Get( i );
			write.Access( i ).ThirdValue = cmp.SecondValue;
		}
	}
};

struct ReaderSystem : ddc::System
{
	ReaderSystem() :
		System( "ReaderSystem" )
	{
		RequireRead<ThirdComponent>();
	}

	virtual void Update( const ddc::UpdateData& data, float delta_t ) override
	{
		ddc::ReadBuffer<ThirdComponent> read = data.Read<ThirdComponent>();

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const ThirdComponent& cmp = read.Get( i );
			int x = cmp.ThirdValue * cmp.ThirdValue;
		}
	}
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
		System( "OnlyReaderSystem" )
	{
		RequireWrite<FirstComponent>();
		RequireWrite<SecondComponent>();
	}

	virtual void Update( const ddc::UpdateData& data, float delta_t ) override
	{
		ddc::ReadBuffer<FirstComponent> read1 = data.Read<FirstComponent>();
		ddc::ReadBuffer<SecondComponent> read2 = data.Read<SecondComponent>();

		for( size_t i = 0; i < read1.Size(); ++i )
		{
			const FirstComponent& cmp1 = read1.Get( i );
			const SecondComponent& cmp2 = read2.Get( i );
			int x = cmp1.FirstValue * cmp2.SecondValue;
		}
	}
};

struct OnlyWriterSystem : ddc::System
{
	OnlyWriterSystem() :
		System( "OnlyWriterSystem" )
	{
		RequireWrite<SecondComponent>();
		RequireWrite<ThirdComponent>();
	}

	virtual void Update( const ddc::UpdateData& data, float delta_t ) override {}
};

TEST_CASE( "EntityManager" )
{
	ddc::World world;

	ddc::Entity a = world.CreateEntity();
	REQUIRE( a.ID == 0 );

	ddc::Entity b = world.CreateEntity();
	REQUIRE( b.ID == 1 );

	ddc::Entity c = world.CreateEntity();
	REQUIRE( c.ID == 2 );

	world.DestroyEntity( a );
	
	ddc::Entity a2 = world.CreateEntity();
	REQUIRE( a2.ID == a.ID );
	REQUIRE( a2.Version == 1 );

	world.DestroyEntity( b );

	ddc::Entity b2 = world.CreateEntity();
	REQUIRE( b2.ID == b.ID );
	REQUIRE( b2.Version == 1 );
}

TEST_CASE( "Component" )
{
	ddc::World world;
	ddc::Entity a = world.CreateEntity();

	bool found = world.HasComponent<FirstComponent>( a );
	REQUIRE( found == false );

	FirstComponent& cmp = world.AddComponent<FirstComponent>( a );
	REQUIRE( cmp.FirstValue == -100 );

	REQUIRE( &cmp.GetType() == &FirstComponent::Type );

	cmp.FirstValue = 5;

	const FirstComponent& cmp2 = *world.AccessComponent<FirstComponent>( a );
	REQUIRE( cmp2.FirstValue == 5 );
	REQUIRE( cmp.FirstValue == cmp2.FirstValue );

	REQUIRE( world.HasComponent<FirstComponent>( a ) );

	world.RemoveComponent<FirstComponent>( a );
	REQUIRE_FALSE( world.HasComponent<FirstComponent>( a ) );
}

TEST_CASE( "Update System" )
{
	ddc::World world;

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e = world.CreateEntity();

		FirstComponent& simple = world.AddComponent<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = world.AddComponent<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	world.RegisterSystem( system );

	world.Update( 0 );

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *world.AccessComponent<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *world.AccessComponent<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update With Discontinuity" )
{
	ddc::World world;

	for( int i = 0; i < 5; ++i )
	{
		ddc::Entity e = world.CreateEntity();

		if( i == 2 )
			continue;
		
		FirstComponent& simple = world.AddComponent<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = world.AddComponent<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	world.RegisterSystem( system );

	world.Update( 0 );

	for( int i = 0; i < 5; ++i )
	{
		if( i == 2 )
			continue;

		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *world.AccessComponent<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *world.AccessComponent<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update Multiple Systems" )
{
	TestSystem a;
	DependentSystem b;

	ddc::World world;
	world.RegisterSystem( a );
	world.RegisterSystem( b );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e = world.CreateEntity();

		FirstComponent& first = world.AddComponent<FirstComponent>( e );
		first.FirstValue = i;

		SecondComponent& second = world.AddComponent<SecondComponent>( e );
		second.SecondValue = -1;

		ThirdComponent& third = world.AddComponent<ThirdComponent>( e );
		third.ThirdValue = -1;
	}

	world.Update( 0.0f );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		SecondComponent& second = *world.AccessComponent<SecondComponent>( e );
		REQUIRE( second.SecondValue == e.ID );
	}

	world.Update( 0 );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		ThirdComponent& third = *world.AccessComponent<ThirdComponent>( e );
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
		ddc::World world;
		world.RegisterSystem( a );
		world.RegisterSystem( b );

		world.Update( 0 );
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
		ddc::World world;
		world.RegisterSystem( a );
		world.RegisterSystem( b );

		world.Update( 0 );
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
		ddc::World world;
		world.RegisterSystem( a );
		world.RegisterSystem( b );
		world.RegisterSystem( c );

		world.Update( 0 );
	}
}

TEST_CASE( "Full Update Loop" )
{
	TestSystem a;
	DependentSystem b;
	ReaderSystem c;

	ddc::System* systems[] = { &a, &b, &c };
	ddc::World world;

	world.RegisterSystem( a );
	world.RegisterSystem( b );
	world.RegisterSystem( c );

	BENCHMARK( "Create Entities" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			ddc::Entity e = world.CreateEntity();
			FirstComponent& first = world.AddComponent<FirstComponent>( e );
			first.FirstValue = i;

			SecondComponent& second = world.AddComponent<SecondComponent>( e );
			second.SecondValue = 0;

			ThirdComponent& third = world.AddComponent<ThirdComponent>( e );
			third.ThirdValue = 0;
		}
	}

	BENCHMARK( "Update" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			world.Update( 0.0f );
		}
	}

	for( int i = 0; i < 1000; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		const SecondComponent* second = world.GetComponent<SecondComponent>( e );
		REQUIRE( second->SecondValue == i );

		const ThirdComponent* third = world.GetComponent<ThirdComponent>( e );
		REQUIRE( third->ThirdValue == i );
	}
}