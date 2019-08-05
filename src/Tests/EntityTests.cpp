#include "PCH.h"
#include "ComponentBuffer.h"
#include "JobSystem.h"
#include "UpdateData.h"
#include "System.h"
#include "SystemsSorting.h"

#include "catch2/catch.hpp"

struct FirstComponent
{
	int FirstValue { -100 };

	DD_BEGIN_CLASS( FirstComponent )
		DD_COMPONENT();
	DD_END_CLASS()
};

struct SecondComponent
{
	int SecondValue { 0 };

	DD_BEGIN_CLASS( SecondComponent )
		DD_COMPONENT();
	DD_END_CLASS()
};

struct ThirdComponent
{
	int ThirdValue { 0 };
	
	DD_BEGIN_CLASS( ThirdComponent )
		DD_COMPONENT();
	DD_END_CLASS()
};

DD_TYPE_CPP( FirstComponent );
DD_TYPE_CPP( SecondComponent );
DD_TYPE_CPP( ThirdComponent );

struct TestSystem : ddc::System
{
	TestSystem() :
		System( "TestSystem" )
	{
		RequireRead<FirstComponent>();
		RequireWrite<SecondComponent>();
	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		auto buffer = data.Data();

		auto read = buffer.Read<FirstComponent>();
		auto write = buffer.Write<SecondComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const FirstComponent& cmp = read[ i ];
			write[ i ].SecondValue = cmp.FirstValue;
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

	virtual void Update( const ddc::UpdateData& data ) override
	{
		auto buffer = data.Data();

		auto read = buffer.Read<SecondComponent>();
		auto write = buffer.Write<ThirdComponent>();

		DD_ASSERT( read.Size() == write.Size() );

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const SecondComponent& cmp = read[ i ];
			write[ i ].ThirdValue = cmp.SecondValue;
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

	virtual void Update( const ddc::UpdateData& data ) override
	{
		auto buffer = data.Data();

		auto read = buffer.Read<ThirdComponent>();

		for( size_t i = 0; i < read.Size(); ++i )
		{
			const ThirdComponent& cmp = read[ i ];
			int x = cmp.ThirdValue * cmp.ThirdValue;
		}
	}
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
		System( "OnlyReaderSystem" )
	{
		RequireRead<FirstComponent>();
		RequireRead<SecondComponent>();
	}

	virtual void Update( const ddc::UpdateData& data ) override
	{
		auto buffer = data.Data();

		auto read1 = buffer.Read<FirstComponent>();
		auto read2 = buffer.Read<SecondComponent>();

		for( size_t i = 0; i < read1.Size(); ++i )
		{
			const FirstComponent& cmp1 = read1[ i ];
			const SecondComponent& cmp2 = read2[ i ];
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

	virtual void Update( const ddc::UpdateData& data ) override {}
};

TEST_CASE( "EntityManager" )
{
	dd::JobSystem jobs( 0 );
	ddc::EntitySpace entities( jobs );

	ddc::Entity a = entities.CreateEntity();
	REQUIRE( a.ID == 0 );

	ddc::Entity b = entities.CreateEntity();
	REQUIRE( b.ID == 1 );

	ddc::Entity c = entities.CreateEntity();
	REQUIRE( c.ID == 2 );

	entities.DestroyEntity( a );
	
	ddc::Entity a2 = entities.CreateEntity();
	REQUIRE( a2.ID == a.ID );
	REQUIRE( a2.Version == 1 );

	entities.DestroyEntity( b );

	ddc::Entity b2 = entities.CreateEntity();
	REQUIRE( b2.ID == b.ID );
	REQUIRE( b2.Version == 1 );
}

TEST_CASE( "Component" )
{
	dd::JobSystem jobs( 0 ); 
	ddc::EntitySpace entities( jobs );
	ddc::Entity a = entities.CreateEntity();

	const dd::TypeInfo* type = DD_FIND_TYPE( FirstComponent );
	REQUIRE( type->ComponentID() != dd::INVALID_COMPONENT );

	bool found = entities.Has<FirstComponent>( a );
	REQUIRE( found == false );

	FirstComponent& cmp = entities.Add<FirstComponent>( a );
	REQUIRE( cmp.FirstValue == -100 );

	cmp.FirstValue = 5;

	const FirstComponent& cmp2 = *entities.Access<FirstComponent>( a );
	REQUIRE( cmp2.FirstValue == 5 );
	REQUIRE( cmp.FirstValue == cmp2.FirstValue );

	REQUIRE( entities.Has<FirstComponent>( a ) );

	entities.Remove<FirstComponent>( a );
	REQUIRE_FALSE( entities.Has<FirstComponent>( a ) );
}

TEST_CASE( "Update System" )
{
	dd::JobSystem jobs( 0 );
	ddc::EntitySpace entities( jobs );

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e = entities.CreateEntity();

		FirstComponent& simple = entities.Add<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = entities.Add<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	entities.RegisterSystem( system );
	entities.Initialize();

	entities.Update( 0 );

	for( int i = 0; i < 8; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *entities.Access<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *entities.Access<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update With Discontinuity" )
{
	dd::TypeInfo::RegisterQueuedTypes();

	dd::JobSystem jobs( 0 );
	ddc::EntitySpace entities( jobs );

	for( int i = 0; i < 5; ++i )
	{
		ddc::Entity e = entities.CreateEntity();

		if( i == 2 )
			continue;
		
		FirstComponent& simple = entities.Add<FirstComponent>( e );
		simple.FirstValue = i;

		SecondComponent& other = entities.Add<SecondComponent>( e );
		other.SecondValue = -1;
	}

	TestSystem system;
	entities.RegisterSystem( system );
	entities.Initialize();

	entities.Update( 0 );

	for( int i = 0; i < 5; ++i )
	{
		if( i == 2 )
			continue;

		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		FirstComponent& simple = *entities.Access<FirstComponent>( e );
		REQUIRE( simple.FirstValue == e.ID );

		SecondComponent& other = *entities.Access<SecondComponent>( e );
		REQUIRE( other.SecondValue == e.ID );
	}
}

TEST_CASE( "Update Multiple Systems" )
{
	TestSystem a;
	DependentSystem b;

	dd::JobSystem jobs( 0 );
	ddc::EntitySpace entities( jobs );
	entities.RegisterSystem( a );
	entities.RegisterSystem( b );
	entities.Initialize();

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e = entities.CreateEntity();

		FirstComponent& first = entities.Add<FirstComponent>( e );
		first.FirstValue = i;

		SecondComponent& second = entities.Add<SecondComponent>( e );
		second.SecondValue = -1;

		ThirdComponent& third = entities.Add<ThirdComponent>( e );
		third.ThirdValue = -1;
	}

	entities.Update( 0.0f );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		SecondComponent& second = *entities.Access<SecondComponent>( e );
		REQUIRE( second.SecondValue == e.ID );
	}

	entities.Update( 0 );

	for( int i = 0; i < 4; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		ThirdComponent& third = *entities.Access<ThirdComponent>( e );
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
		b.DependsOn( a );

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
		b.DependsOn( a );

		TestSystem c;
		c.DependsOn( b );

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
		b.DependsOn( a );

		TestSystem c;
		c.DependsOn( a );

		TestSystem d;
		d.DependsOn( b );
		d.DependsOn( c );

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
		b.DependsOn( a );

		ddc::System* systems[] = { &a, &b };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );

		dd::JobSystem jobsystem( 0u );
		ddc::EntitySpace entities( jobsystem );
		entities.RegisterSystem( a );
		entities.RegisterSystem( b );
		entities.Initialize();

		entities.Update( 0 );
	}

	SECTION( "Multiple Roots" )
	{
		TestSystem a;
		TestSystem b;

		TestSystem c;
		c.DependsOn( a );
		c.DependsOn( b );

		ddc::System* systems[] = { &a, &b, &c };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );

		dd::JobSystem jobsystem( 0u );
		ddc::EntitySpace entities( jobsystem );
		entities.RegisterSystem( a );
		entities.RegisterSystem( b );
		entities.Initialize();

		entities.Update( 0 );
	}

	SECTION( "Diamond" )
	{
		TestSystem a;

		TestSystem b;
		TestSystem c;
		c.DependsOn( a );
		c.DependsOn( b );

		TestSystem d;
		d.DependsOn( c );

		ddc::System* systems[] = { &a, &b, &c, &d };

		dd::Span<ddc::System*> span_systems( systems );

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies( span_systems, ordered );

		REQUIRE( ordered[ 0 ].m_system == &a );
		REQUIRE( ordered[ 1 ].m_system == &b );
		REQUIRE( ordered[ 2 ].m_system == &c );
		REQUIRE( ordered[ 3 ].m_system == &d );

		dd::JobSystem jobsystem( 0u );
		ddc::EntitySpace entities( jobsystem );
		entities.RegisterSystem( a );
		entities.RegisterSystem( b );
		entities.RegisterSystem( c );
		entities.Initialize();

		entities.Update( 0 );
	}
}

TEST_CASE( "Full Update Loop" )
{
	TestSystem a;
	DependentSystem b;
	ReaderSystem c;

	ddc::System* systems[] = { &a, &b, &c };
	
	dd::JobSystem jobsystem( 0u );
	ddc::EntitySpace entities( jobsystem );

	entities.RegisterSystem( a );
	entities.RegisterSystem( b );
	entities.RegisterSystem( c );
	entities.Initialize();

	BENCHMARK( "Create Entities" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			ddc::Entity e = entities.CreateEntity();
			FirstComponent& first = entities.Add<FirstComponent>( e );
			first.FirstValue = i;

			SecondComponent& second = entities.Add<SecondComponent>( e );
			second.SecondValue = 0;

			ThirdComponent& third = entities.Add<ThirdComponent>( e );
			third.ThirdValue = 0;
		}
	}

	BENCHMARK( "Update" )
	{
		for( int i = 0; i < 1000; ++i )
		{
			entities.Update( 0.0f );
		}
	}

	for( int i = 0; i < 1000; ++i )
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 0;

		const SecondComponent* second = entities.Get<SecondComponent>( e );
		REQUIRE( second->SecondValue == i );

		const ThirdComponent* third = entities.Get<ThirdComponent>( e );
		REQUIRE( third->ThirdValue == i );
	}
}