//
// EntityManagerTests.h - Tests for Entitys_manager->
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "EntityManager.h"
#include "ComponentBase.h"
#include "DenseVectorPool.h"


using namespace dd;

class FooComponent : public ComponentBase
{
public:
	int A;

	typedef DenseVectorPool<FooComponent> Pool;

	BEGIN_TYPE( FooComponent )
		MEMBER( FooComponent, A )
	END_TYPE
};

class BarComponent : public ComponentBase
{
public:
	float B;

	typedef DenseVectorPool<BarComponent> Pool;

	BEGIN_TYPE( BarComponent )
		MEMBER( BarComponent, B )
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

	Services::RegisterComponent<FooComponent>();
	Services::RegisterComponent<BarComponent>();

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

		Vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		Swap();

		Vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.Size() == 2 );
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

		Vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

		REQUIRE( handles.Size() == 2 );
		REQUIRE( !handles.Contains( bar ) );
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

	Vector<EntityHandle> handles = s_manager->FindAllWithReadable<FooComponent>();

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

		Vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle bar = s_manager->Create();
		s_manager->AddComponent<BarComponent>( bar );

		Swap();

		Vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo );

		EntityHandle foo2 = s_manager->Create();
		s_manager->AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 2 );
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

		Vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 2 );
		REQUIRE( !handles.Contains( bar ) );
	}

	SECTION( "Multiple Components" )
	{
		EntityHandle foobar = s_manager->Create();		
		s_manager->AddComponent<FooComponent>( foobar );
		s_manager->AddComponent<BarComponent>( foobar );

		Swap();

		Vector<EntityHandle> handles;
		s_manager->ForAllWithReadable<FooComponent, BarComponent>( [&handles]( EntityHandle h, ComponentHandle<FooComponent> f, ComponentHandle<BarComponent> b ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
	}
}

TEST_CASE( "[EntityHandle] Unpack" )
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
	foobar.Unpack( foo2, bar2 );

	REQUIRE( foo1.Write()->A == foo2.Write()->A );
	REQUIRE( bar1.Write()->B == bar2.Write()->B );
}