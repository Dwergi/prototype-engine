//
// EntityManagerTests.h - Tests for EntityManager.
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "EntityManager.h"
#include "Component.h"
#include "DenseVectorPool.h"


using namespace dd;

class FooComponent : public Component
{
public:
	int A;

	typedef DenseVectorPool<FooComponent> Pool;

	BEGIN_TYPE( FooComponent )
		MEMBER( FooComponent, A )
	END_TYPE
};

class BarComponent : public Component
{
public:
	float B;

	typedef DenseVectorPool<BarComponent> Pool;

	BEGIN_TYPE( BarComponent )
		MEMBER( BarComponent, B )
	END_TYPE
};

void SetUp()
{
	REGISTER_TYPE( FooComponent ); 
	REGISTER_TYPE( BarComponent );

	Services::RegisterComponent<FooComponent>();
	Services::RegisterComponent<BarComponent>();

	auto& fooBuffer = Services::GetDoubleBuffer<FooComponent>();
	fooBuffer.Clear();

	auto& barBuffer = Services::GetDoubleBuffer<BarComponent>();
	barBuffer.Clear();
}

void Swap()
{
	Services::GetDoubleBuffer<FooComponent>().Swap();
	Services::GetDoubleBuffer<FooComponent>().Duplicate();

	Services::GetDoubleBuffer<BarComponent>().Swap();
	Services::GetDoubleBuffer<BarComponent>().Duplicate();
}

TEST_CASE( "[EntityManager] Create" )
{
	SetUp();

	EntityManager manager;
	EntityHandle handle = manager.Create();
	manager.AddComponent<FooComponent>( handle );

	Swap();

	REQUIRE( manager.HasComponent<FooComponent>( handle ) );
	
	manager.RemoveComponent<FooComponent>( handle );

	Swap();

	REQUIRE_FALSE( manager.HasComponent<FooComponent>( handle ) );
}

TEST_CASE( "[EntityManager] FindAllWith" )
{
	SetUp();

	EntityManager manager;
	
	SECTION( "Single Entity" )
	{
		EntityHandle handle = manager.Create();
		manager.AddComponent<FooComponent>( handle );

		Swap();

		Vector<EntityHandle> handles = manager.FindAllWith<FooComponent>();

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle bar = manager.Create();
		manager.AddComponent<BarComponent>( bar );

		Swap();

		Vector<EntityHandle> handles = manager.FindAllWith<FooComponent>();

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle foo2 = manager.Create();
		manager.AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles = manager.FindAllWith<FooComponent>();

		REQUIRE( handles.Size() == 2 );
	}

	SECTION( "Multiple Valid Entities" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle bar = manager.Create();
		manager.AddComponent<BarComponent>( bar );

		EntityHandle foo2 = manager.Create();
		manager.AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles = manager.FindAllWith<FooComponent>();

		REQUIRE( handles.Size() == 2 );
		REQUIRE( !handles.Contains( bar ) );
	}
}

TEST_CASE( "[EntityManager] Single Instance of Component" )
{
	SetUp();

	EntityManager manager;
	EntityHandle handle = manager.Create();

	FooComponent& foo = *manager.AddComponent<FooComponent>( handle );
	foo.A = 0;

	Swap();

	FooComponent& foo1 = manager.GetComponent<FooComponent>( handle );

	foo.A = 1;
	REQUIRE( foo1.A == foo.A );

	Vector<EntityHandle> handles = manager.FindAllWith<FooComponent>();

	foo1.A = 2;

	FooComponent& foo2 = handles[ 0 ].Get<FooComponent>();
	REQUIRE( foo.A == foo1.A );
	REQUIRE( foo1.A == foo2.A );

	foo.A = 3;

	manager.ForAllWith<FooComponent>( [&foo]( EntityHandle h, FooComponent& f ) { REQUIRE( f.A == foo.A ); } );
}

TEST_CASE( "[EntityManager] ForAllWith" )
{
	SetUp();

	EntityManager manager;

	SECTION( "Single Entity" )
	{
		EntityHandle handle = manager.Create();
		manager.AddComponent<FooComponent>( handle );

		Swap();

		Vector<EntityHandle> handles;
		manager.ForAllWith<FooComponent>( [&handles]( EntityHandle h, FooComponent& f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == handle );
	}

	SECTION( "Single Valid Entity" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle bar = manager.Create();
		manager.AddComponent<BarComponent>( bar );

		Swap();

		Vector<EntityHandle> handles;
		manager.ForAllWith<FooComponent>( [&handles]( EntityHandle h, FooComponent& f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
		REQUIRE( handles[ 0 ] == foo );
	}

	SECTION( "Multiple Entities" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle foo2 = manager.Create();
		manager.AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles;
		manager.ForAllWith<FooComponent>( [&handles]( EntityHandle h, FooComponent& f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 2 );
	}

	SECTION( "Multiple Valid Entities" )
	{
		EntityHandle foo = manager.Create();
		manager.AddComponent<FooComponent>( foo );

		EntityHandle bar = manager.Create();
		manager.AddComponent<BarComponent>( bar );

		EntityHandle foo2 = manager.Create();
		manager.AddComponent<FooComponent>( foo2 );

		Swap();

		Vector<EntityHandle> handles;
		manager.ForAllWith<FooComponent>( [&handles]( EntityHandle h, FooComponent& f ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 2 );
		REQUIRE( !handles.Contains( bar ) );
	}

	SECTION( "Multiple Components" )
	{
		EntityHandle foobar = manager.Create();		
		manager.AddComponent<FooComponent>( foobar );
		manager.AddComponent<BarComponent>( foobar );

		Swap();

		Vector<EntityHandle> handles;
		manager.ForAllWith<FooComponent, BarComponent>( [&handles]( EntityHandle h, FooComponent& f, BarComponent& b ) { handles.Add( h ); } );

		REQUIRE( handles.Size() == 1 );
	}
}

TEST_CASE( "[EntityHandle] Unpack" )
{
	SetUp();

	EntityManager manager;

	EntityHandle foobar = manager.Create();
	manager.AddComponent<FooComponent>( foobar );
	manager.AddComponent<BarComponent>( foobar );

	Swap();

	FooComponent& rfoo = manager.GetComponent<FooComponent>( foobar );
	rfoo.A = 1;
	BarComponent& rbar = manager.GetComponent<BarComponent>( foobar );
	rbar.B = 3.0f;

	FooComponent foo;
	BarComponent bar;
	foobar.Unpack( foo, bar );

	REQUIRE( foo.A == rfoo.A );
	REQUIRE( bar.B == rbar.B );
}