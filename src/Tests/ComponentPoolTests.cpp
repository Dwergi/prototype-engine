#include "PrecompiledHeader.h"

#include "catch/catch.hpp"

#include "PackedPool.h"

struct FooComponent
	: public dd::IComponent
{
	int ID;
};

template <typename PoolType>
void CreateEntities( dd::Vector<dd::EntityHandle>& handles, PoolType& pool, int start, int end )
{
	for( int i = start; i < end; ++i )
	{
		dd::EntityHandle handle;
		memset( &handle, 0, sizeof( dd::EntityHandle ) );

		*(int*) (&handle.Handle) = i;
		handles.Add( handle );

		FooComponent* cmp = pool.Create( handle );
		REQUIRE( cmp != nullptr );

		cmp->ID = i;
	}
}

const int ComponentCount = 100;

template <typename PoolType>
void TestPool()
{
	PoolType pool;

	dd::Vector<dd::EntityHandle> handles;

	CreateEntities( handles, pool, 0, ComponentCount );

	REQUIRE( pool.Size() == ComponentCount );

	for( int i = 0; i < ComponentCount; ++i )
	{
		bool exists = pool.Exists( handles[i] );
		REQUIRE( exists );

		FooComponent* cmp = pool.Find( handles[i] );
		REQUIRE( cmp != nullptr );
		REQUIRE( cmp->ID == i );
	}

	int count = 0;
	for( FooComponent& cmp : pool )
	{
		++count;
	}

	REQUIRE( count == ComponentCount );

	for( int i = 0; i < ComponentCount; ++i )
	{
		pool.Remove( handles[i] );

		REQUIRE( !pool.Exists( handles[i] ) );
	}

	REQUIRE( pool.Size() == 0 );

	CreateEntities( handles, pool, 0, ComponentCount );

	const int start = ComponentCount / 4;
	const int end = start + ComponentCount / 2;
	for( int i = start; i < end; ++i )
	{
		pool.Remove( handles[i] );
		REQUIRE( !pool.Exists( handles[i] ) );
	}

	CreateEntities( handles, pool, start, end );

	for( int i = 0; i < ComponentCount; ++i )
	{
		REQUIRE( pool.Exists( handles[ i ] ) );

		FooComponent* cmp = pool.Find( handles[ i ] );
		REQUIRE( cmp != nullptr );
		REQUIRE( cmp->ID == i );
	}
}

TEST_CASE( "[ComponentPool] Packed Pool" )
{
	TestPool<dd::PackedPool<FooComponent>>();
}