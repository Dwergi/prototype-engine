#include "PrecompiledHeader.h"

#include "catch/catch.hpp"

#include "ComponentPoolTestComponent.h"
#include "SortedVectorPool.h"
#include "DenseMapPool.h"
#include "DenseVectorPool.h"

struct TestComponent
	: public dd::ComponentBase
{
	int ID;
};

template <typename PoolType>
void CreateEntities( dd::Vector<dd::EntityHandle>& handles, PoolType& pool, int count )
{
	for( int i = 0; i < count; ++i )
	{
		dd::EntityHandle handle;
		memset( &handle, 0, sizeof( dd::EntityHandle ) );

		*(int*) (&handle) = i;
		handles.Add( handle );

		TestComponent* cmp = pool.Create( handle );
		REQUIRE( cmp != nullptr );

		cmp->ID = i;
	}
}

template <typename PoolType>
void TestPool()
{
	PoolType pool;

	dd::Vector<dd::EntityHandle> handles;

	CreateEntities( handles, pool, 10 );

	REQUIRE( pool.Size() == 10 );

	for( int i = 0; i < 10; ++i )
	{
		bool exists = pool.Exists( handles[i] );
		REQUIRE( exists );

		TestComponent* cmp = pool.Find( handles[i] );
		REQUIRE( cmp != nullptr );
		REQUIRE( cmp->ID == i );
	}

	int count = 0;
	for( TestComponent& cmp : pool )
	{
		++count;
	}

	REQUIRE( count == 10 );

	for( int i = 0; i < 10; ++i )
	{
		pool.Remove( handles[i] );

		REQUIRE( !pool.Exists( handles[i] ) );
	}

	REQUIRE( pool.Size() == 0 );

	CreateEntities( handles, pool, 10 );

	pool.Remove( handles[5] );

	REQUIRE( !pool.Exists( handles[5] ) );

	for( TestComponent& cmp : pool )
	{
		REQUIRE( cmp.Entity != handles[5] );
	}
}

TEST_CASE( "Dense Vector Pool" )
{
	TestPool<dd::DenseVectorPool<TestComponent>>();
}

TEST_CASE( "Unordered Map Pool" )
{
	TestPool<dd::DenseMapPool<TestComponent>>();
}

TEST_CASE( "Sorted Vector Pool" )
{
	TestPool<dd::SortedVectorPool<TestComponent>>();
}