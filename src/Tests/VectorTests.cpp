//
// VectorTests.cpp - Tests for vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "String_dd.h"
#include "Vector.h"

#include "TestTypes.h"

TEST_CASE( "[Vector] Add" )
{
	dd::Vector<int> container;

	REQUIRE( container.Size() == 0 );

	container.Add( 123 );

	REQUIRE( container.Size() == 1 );
	REQUIRE( container[ 0 ] == 123 );
}

TEST_CASE( "[Vector] AddAll" )
{
	dd::Vector<int> container;
	dd::Vector<int> other;

	for( int i = 0; i < 32; ++i )
	{
		other.Add( i );
	}

	REQUIRE( other.Size() == 32 );

	container.AddAll( other );

	REQUIRE( container.Size() == 32 );
	REQUIRE( container[ 0 ] == 0 );
	REQUIRE( container[ 31 ] == 31 );

	container.AddAll( other );

	REQUIRE( container.Size() == 64 );
	REQUIRE( container[ 63 ] == 31 );
}

TEST_CASE( "[Vector] Insert" )
{
	dd::Vector<int> container;

	// insert at start
	for( int i = 0; i < 32; ++i )
	{
		container.Insert( i, 0 );
	}
	
	for( int i = 0; i < 32; ++i )
	{
		REQUIRE( container[i] == 32 - i - 1 );
	}

	container.Clear();

	// insert at end
	for( int i = 0; i < 32; ++i )
	{
		container.Insert( i, i );
	}
	
	for( int i = 0; i < 32; ++i )
	{
		REQUIRE( container[i] == i );
	}

	for( int i = 0; i < 32; ++i )
	{
		container.Insert( i, i * 2 );
	}

	int prev;
	int prev_prev;
	for( int i = 3; i < 64; i += 2 )
	{
		prev = container[i - 1];
		prev_prev = container[i - 2];

		REQUIRE( container[i] == prev );
		REQUIRE( container[i] == prev_prev + 1 );
	}
}

TEST_CASE( "[Vector] Remove" )
{
	dd::Vector<int> container;

	container.Add( 0 );

	container.Remove( 0 );
	REQUIRE( container.Size() == 0 );

	for( int i = 0; i < 32; ++i )
	{
		container.Add( i );
	}

	container.RemoveOrdered( 0 );

	REQUIRE( container[ 0 ] == 1 );
	REQUIRE( container[ 30 ] == 31 );
	REQUIRE( container.Size() == 31 );

	container.Clear();
	
	REQUIRE( container.Size() == 0 );
}

TEST_CASE( "[Vector] Contains" )
{
	dd::Vector<int> container;

	for( int i = 0; i < 32; ++i )
	{
		container.Add( i );
	}

	REQUIRE( container.Find( 15 ) == 15 );
	REQUIRE( container.Contains( 15 ) );
}

TEST_CASE( "[Vector] Struct" )
{
	dd::Vector<Test::NestedStruct> container;

	for( int i = 0; i < 128; ++i )
	{
		container.Add( Test::NestedStruct() );
	}

	REQUIRE( container.Size() == 128 );

	container.Pop();
}

TEST_CASE( "[Vector] Iteration" )
{
	dd::Vector<int> container;

	for( int i = 0; i < 128; ++i )
	{
		container.Add( i );
	}

	int current = 0;
	for( int i : container )
	{
		REQUIRE( i == current );
		++current;
	}

	const dd::Vector<int>& const_ref = container;

	current = 0;
	for( int i : const_ref )
	{
		REQUIRE( i == current );
		++current;
	}
}

TEST_CASE( "[Vector] Assignment" )
{
	dd::Vector<int> a;
	dd::Vector<int> b;

	for( int i = 0; i < 128; ++i )
	{
		a.Add( i );
	}

	b = a;

	for( int i = 0; i < 128; ++i )
	{
		REQUIRE( a[ i ] == b[ i ] );
	}
}

TEST_CASE( "[Vector] Out of Scope" )
{
	dd::Vector<int> b;
	{
		dd::Vector<int> a;
		for( int i = 0; i < 128; ++i )
		{
			a.Add( i );
		}

		b = a;
	}

	for( int i = 0; i < 128; ++i )
	{
		REQUIRE( b[ i ] == i );
	}
}

TEST_CASE( "[Vector] Reserve" )
{
	dd::Vector<int> a;
	a.Reserve( 128 );

	REQUIRE( a.Capacity() == 128 );
	REQUIRE( a.Size() == 0 );

	a.Reserve( 5 );

	REQUIRE( a.Capacity() == 128 );
	REQUIRE( a.Size() == 0 );

	for( int i = 0; i < 100; ++i )
	{
		a.Add( i );
	}

	REQUIRE( a.Capacity() == 128 );
	REQUIRE( a.Size() == 100 );
}

TEST_CASE( "[Vector] Resize" )
{
	dd::Vector<int> a;
	a.Resize( 128 );

	a[127] = 0;

	REQUIRE( a.Size() == 128 );
	REQUIRE( a.Capacity() == 128 );

	a.Resize( 64 );

	REQUIRE( a.Size() == 64 );
	REQUIRE( a.Capacity() == 64 );

	a.Resize( 0 );

	REQUIRE( a.Size() == 0 );
	REQUIRE( a.Capacity() == 0 );

	a.Resize( 128 );

	REQUIRE( a.Size() == 128 );
	REQUIRE( a.Capacity() == 128 );
}

TEST_CASE( "[Vector] Reverse" )
{
	dd::Vector<int> a;

	for( int i = 0; i < 32; ++i )
	{
		a.Add( i );
	}

	a.Reverse();

	for( int i = 0; i < 32; ++i )
	{
		REQUIRE( a[i] == (32 - i - 1) );
	}

	// odd-sized
	a.Insert( 32, 0 );

	a.Reverse();

	for( int i = 0; i < 33; ++i )
	{
		REQUIRE( a[i] == i );
	}
}

TEST_CASE( "[StackVector] Basic" )
{
	dd::StackVector<int, 32> a;

	for( int i = 0; i < 32; ++i )
	{
		a.Add( i );
	}

	REQUIRE( a.IsOnStack() );

	a.Add( 32 );

	REQUIRE( !a.IsOnStack() );

	for( int i = 0; i < 33; ++i )
	{
		REQUIRE( a[i] == i );
	}
}

TEST_CASE( "[StackVector] Copy" )
{
	dd::StackVector<int, 16> b;

	for( int i = 0; i < 16; ++i )
	{
		b.Add( i );
	}

	REQUIRE( b.IsOnStack() );

	dd::StackVector<int, 32> a( b );

	REQUIRE( a.IsOnStack() );

	for( int i = 0; i < 16; ++i )
	{
		REQUIRE( a[i] == i );
	}
}