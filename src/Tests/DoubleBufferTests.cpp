//
// DoubleBufferTests.h - Tests for DoubleBuffer.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "DoubleBuffer.h"

#include "Component.h"
#include "DenseVectorPool.h"
#include "EntityManager.h"

using namespace dd;

TEST_CASE( "[DoubleBuffer] Swap" )
{
	Vector<int> v1, v2;
	DoubleBuffer<Vector<int>> buffer( &v1, &v2 );

	{
		Vector<int>& write = buffer.GetWrite();

		for( int i = 0; i < 16; ++i )
		{
			write.Add( i );
		}
	}

	buffer.Swap();

	{
		const Vector<int>& read = buffer.GetRead();
		for( int i = 0; i < 16; ++i )
		{
			REQUIRE( read[i] == i );
		}
	}
}

TEST_CASE( "[DoubleBuffer] Duplicate" )
{
	Vector<int> v1, v2;
	DoubleBuffer<Vector<int>> buffer( &v1, &v2 );

	{
		Vector<int>& write = buffer.GetWrite();

		for( int i = 0; i < 16; ++i )
		{
			write.Add( i );
		}
	}

	buffer.Swap();
	buffer.Duplicate();

	{
		Vector<int>& write = buffer.GetWrite();
		for( int i = 0; i < 16; ++i )
		{
			REQUIRE( write[i] == i );
		}
	}
}

class TestComponent : public Component
{
public:
	int A;

	typedef DenseVectorPool<TestComponent> Pool;

	BEGIN_TYPE( TestComponent )
		MEMBER( TestComponent, A )
	END_TYPE
};

TEST_CASE( "[DoubleBuffer] Components" )
{
	EntityManager system;
	EntityHandle handle = system.Create();

	REGISTER_TYPE( TestComponent );
	
	TestComponent::Pool pool;
	pool.Create( handle );

	ComponentHandle<TestComponent> cmp_handle( handle, pool );
	
	REQUIRE( cmp_handle.IsValid() );
}