//
// StackArrayTests.h - Tests for StackArray.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "StackArray.h"

TEST_CASE( "[StackArray] Push" )
{
	dd::StackArray<int, 64> container;

	REQUIRE( container.Size() == 0 );

	container.Push( 123 );

	REQUIRE( container.Size() == 1 );
	REQUIRE( container[ 0 ] == 123 );
}

TEST_CASE( "[StackArray] PushAll" )
{
	dd::StackArray<int, 64> container;
	dd::StackArray<int, 32> other;

	for( int i = 0; i < 32; ++i )
	{
		other.Push( i );
	}

	REQUIRE( other.Size() == 32 );

	container.PushAll( other );

	REQUIRE( container.Size() == 32 );
	REQUIRE( container[ 0 ] == 0 );
	REQUIRE( container[ 31 ] == 31 );

	container.PushAll( other );

	REQUIRE( container.Size() == 64 );
	REQUIRE( container[ 63 ] == 31 );
}

TEST_CASE( "[StackArray] Pop" )
{
	dd::StackArray<int, 32> container;

	container.Push( 0 );

	REQUIRE( container.Pop() == 0 );
	REQUIRE( container.Size() == 0 );

	for( int i = 0; i < 32; ++i )
	{
		container.Push( i );
	}

	REQUIRE( container.Pop() == 31 );
	REQUIRE( container.Size() == 31 );
}

TEST_CASE( "[StackArray] Contains" )
{
	dd::StackArray<int, 32> container;

	for( int i = 0; i < 32; ++i )
	{
		container.Push( i );
	}

	REQUIRE( container.IndexOf( 15 ) == 15 );
	REQUIRE( container.Contains( 15 ) );
}

struct SimpleStruct
{
	int Integer;
	float Float;
	std::string String;
};

struct ComplexStruct
{
	SimpleStruct Nested;
	int OtherInt;
};

TEST_CASE( "[StackArray] Struct" )
{
	dd::StackArray<ComplexStruct, 128> container;

	for( int i = 0; i < 128; ++i )
	{
		container.Push( ComplexStruct() );
	}

	REQUIRE( container.Size() == 128 );
}