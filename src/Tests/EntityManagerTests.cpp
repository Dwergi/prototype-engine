//
// EntityManagerTests.h - Tests for Entitys_manager->
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

#include "EntityManager.h"
#include "IComponent.h"
#include "PackedPool.h"

using namespace dd;

class FooComponent : public IComponent
{
public:
	int A;

	typedef PackedPool<FooComponent> Pool;

	BEGIN_TYPE( FooComponent )
		MEMBER( FooComponent, A )
	END_TYPE
};

class BarComponent : public IComponent
{
public:
	float B;

	typedef PackedPool<BarComponent> Pool;

	BEGIN_TYPE( BarComponent )
		MEMBER( BarComponent, B )
	END_TYPE
};

class DenseVectorComponent : public IComponent
{
public: 
	int Integer;

	using Pool = PackedPool<DenseVectorComponent>;

	BEGIN_TYPE( DenseVectorComponent )
		MEMBER( DenseVectorComponent, Integer )
	END_TYPE
};

class SortedVectorComponent : public IComponent
{
public:
	int Integer;

	using Pool = PackedPool<SortedVectorComponent>;

	BEGIN_TYPE( SortedVectorComponent )
		MEMBER( SortedVectorComponent, Integer )
	END_TYPE
};

class DenseMapComponent : public IComponent
{
public:
	int Integer;

	using Pool = PackedPool<DenseMapComponent>;

	BEGIN_TYPE( DenseMapComponent )
		MEMBER( DenseMapComponent, Integer )
	END_TYPE
};

EntityManager* s_manager = nullptr;

void SetUp()
{
	if( s_manager != nullptr )
	{
		delete s_manager;
	}

	s_manager = new EntityManager();

	REGISTER_TYPE( FooComponent ); 
	REGISTER_TYPE( BarComponent );
	
	s_manager->RegisterComponent<FooComponent>();
	s_manager->RegisterComponent<BarComponent>();
}

void Swap()
{
	s_manager->Update( 1.0f );
}

TEST_CASE( "[EntityManager] Create" )
{
	SetUp();

	EntityHandle handle = s_manager->Create();
	s_manager->AddComponent<FooComponent>( handle );

	REQUIRE( s_manager->HasWritable<FooComponent>( handle ) );

	Swap();

	REQUIRE( s_manager->HasReadable<FooComponent>( handle ) );
	
	s_manager->RemoveComponent<FooComponent>( handle );

	REQUIRE_FALSE( s_manager->HasWritable<FooComponent>( handle ) );

	Swap();

	REQUIRE_FALSE( s_manager->HasReadable<FooComponent>( handle ) );
}

TEST_CASE( "[EntityManager] FindAllWithReadable" )
{
	SetUp();

	SECTION( "Single Entity" )
	{
		EntityHandle handle = s_manager->Create();
		s_manager->AddComponent<FooComponent>( handle );

		Swap();

		std::vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		Swap();

		std::vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		std::vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.size() == 2 );
	}

	SECTION( "Multiple Valid Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		std::vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.size() == 2 );
		REQUIRE( std::find( handles.begin(), handles.end(), bar ) == handles.end() );
	}
}

TEST_CASE( "[EntityManager] Single Instance of Component" )
{
	SetUp();

	EntityManager manager;
	EntityHandle handle = s_manager->Create();

	ComponentHandle<FooComponent> foo = s_manager->AddComponent<FooComponent>( handle );

	Swap();

	foo.Write()->A = 0;

	ComponentHandle<FooComponent> foo1 = s_manager->GetComponent<FooComponent>( handle );

	foo.Write()->A = 1;
	REQUIRE( foo1.Write()->A == foo.Write()->A );

	std::vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

	foo1.Write()->A = 2;

	ComponentHandle<FooComponent> foo2 = handles[ 0 ].Get<FooComponent>();
	REQUIRE( foo.Write()->A == foo1.Write()->A );
	REQUIRE( foo1.Write()->A == foo2.Write()->A );

	foo.Write()->A = 3;

	s_manager->ForAllWithReadable<FooComponent>( [&foo]( EntityHandle h, ComponentHandle<FooComponent> f ) { REQUIRE( f.Write()->A == foo.Write()->A ); } );
}

TEST_CASE( "[EntityManager] ForAllWithReadable" )
{
	SetUp();

	EntityManager manager;

	SECTION( "Single Entity" )
	{
		EntityHandle handle = s_manager->Create();
		s_manager->AddComponent<FooComponent>( handle );

		Swap();

		std::vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.push_back( h ); } );

		REQUIRE( handles.size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		Swap();

		std::vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.push_back( h ); } );

		REQUIRE( handles.size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		std::vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.push_back( h ); } );

		REQUIRE( handles.size() == 2 );
	}

	SECTION( "Multiple Valid Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		std::vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.push_back( h ); } );

		REQUIRE( handles.size() == 2 );
		REQUIRE( std::find( handles.begin(), handles.end(), bar ) == handles.end() );
	}

	SECTION( "Multiple Components" )
	{
		EntityHandle foobar = s_manager->Create();		
		s_manager->AddComponent<FooComponent>( foobar );
		s_manager->AddComponent<BarComponent>( foobar );

		Swap();

		std::vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent, BarComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f, ComponentHandle<BarComponent> b ) { handles.push_back( h ); } );

		REQUIRE( handles.size() == 1 );
	}
}

TEST_CASE( "[EntityHandle] GetAll" )
{
	SetUp();

	EntityManager manager;

	EntityHandle foobar = s_manager->Create();
	s_manager->AddComponent<FooComponent>( foobar );
	s_manager->AddComponent<BarComponent>( foobar );

	Swap();

	ComponentHandle<FooComponent> foo1 = s_manager->GetComponent<FooComponent>( foobar );
	foo1.Write()->A = 1;
	ComponentHandle<BarComponent> bar1 = s_manager->GetComponent<BarComponent>( foobar );
	bar1.Write()->B = 3.0f;

	ComponentHandle<FooComponent> foo2;
	ComponentHandle<BarComponent> bar2;
	foobar.GetAll( foo2, bar2 );

	REQUIRE( foo1.Write()->A == foo2.Write()->A );
	REQUIRE( bar1.Write()->B == bar2.Write()->B );
}

TEST_CASE( "[EntityManager] Destroy" )
{
	EntityHandle foo = s_manager->Create();
	s_manager->AddComponent<FooComponent>( foo );

	REQUIRE( foo.Has<FooComponent>() );
	s_manager->Destroy( foo );

	REQUIRE( !foo.IsValid() );
}

TEST_CASE( "[EntityManager] Stress Test" )
{
	return; // comment to run this, beware, it takes a minute or so

	const int MaxItems = 32 * 1024;
	const int Iterations = 5;
	const int Seed = 0;
	srand( Seed );

	TypeInfo::RegisterComponent<DenseMapComponent>( "DenseMapComponent" );
	s_manager->RegisterComponent<DenseMapComponent>();

	TypeInfo::RegisterComponent<DenseVectorComponent>( "DenseVectorComponent" );
	s_manager->RegisterComponent<DenseVectorComponent>();

	TypeInfo::RegisterComponent<SortedVectorComponent>( "SortedVectorComponent" );
	s_manager->RegisterComponent<SortedVectorComponent>();

	std::vector<EntityHandle> entities;
	entities.reserve( MaxItems );

	for( int iteration = 0; iteration < Iterations; ++iteration )
	{
		BENCHMARK( "Entity Manager Add" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				EntityHandle entity = s_manager->Create();
				entities.push_back( entity );
			}
		}

		BENCHMARK( "Dense Map Add" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					s_manager->AddComponent<DenseMapComponent>( entities[ i ] );
				}
			}
		}

		BENCHMARK( "Dense Vector Add" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					s_manager->AddComponent<DenseVectorComponent>( entities[ i ] );
				}
			}
		}

		BENCHMARK( "Sorted Vector Add" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					s_manager->AddComponent<SortedVectorComponent>( entities[ i ] );
				}
			}
		}


		s_manager->Update( 1.0f );

		BENCHMARK( "Dense Map Remove" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					EntityHandle entity = entities[ i ];
					if( entity.Has<DenseMapComponent>() )
					{
						s_manager->RemoveComponent<DenseMapComponent>( entity );
					}
				}
			}
		}

		BENCHMARK( "Dense Vector Remove" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					EntityHandle entity = entities[ i ];
					if( entity.Has<DenseVectorComponent>() )
					{
						s_manager->RemoveComponent<DenseVectorComponent>( entity );
					}
				}
			}
		}

		BENCHMARK( "Sorted Vector Remove" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				if( rand() % 2 == 0 )
				{
					EntityHandle entity = entities[ i ];
					if( entity.Has<SortedVectorComponent>() )
					{
						s_manager->RemoveComponent<SortedVectorComponent>( entity );
					}
				}
			}
		}

		s_manager->Update( 1.0f );

		BENCHMARK( "Destroy" )
		{
			for( int i = 0; i < MaxItems; ++i )
			{
				s_manager->Destroy( entities[ i ] );
			}
		}

		s_manager->Update( 1.0f );

		entities.clear();
	}
}