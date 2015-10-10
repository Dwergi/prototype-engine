//
// VectorTests.cpp - Tests for vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

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