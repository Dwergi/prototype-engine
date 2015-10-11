//
// StackArrayTests.h - Tests for Array.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "TestTypes.h"

namespace ArrayTests
{
	TEST_CASE( "[Array] Push" )
	{
		dd::Array<int, 64> container;

		REQUIRE( container.Size() == 0 );

		container.Push( 123 );

		REQUIRE( container.Size() == 1 );
		REQUIRE( container[ 0 ] == 123 );
	}

	TEST_CASE( "[Array] PushAll" )
	{
		dd::Array<int, 64> container;
		dd::Array<int, 32> other;

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

	TEST_CASE( "[Array] Pop" )
	{
		dd::Array<int, 32> container;

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

	TEST_CASE( "[Array] Contains" )
	{
		dd::Array<int, 32> container;

		for( int i = 0; i < 32; ++i )
		{
			container.Push( i );
		}

		REQUIRE( container.IndexOf( 15 ) == 15 );
		REQUIRE( container.Contains( 15 ) );
	}

	TEST_CASE( "[Array] Struct" )
	{
		dd::Array<Test::NestedStruct, 32> container;
	
		ASSERT( sizeof( Test::NestedStruct ) > sizeof( Test::SimpleStruct ) );
		ASSERT( sizeof( dd::Array<Test::NestedStruct, 32> ) > sizeof( Test::NestedStruct ) * 32 );

		for( int i = 0; i < 32; ++i )
		{
			Test::NestedStruct s;
			s.Nested.Int = i;
			s.Nested.Flt = (float) i;
			s.Nested.Str = "test";
			s.SecondInt = i;
			container.Push( s );
		}

		REQUIRE( container.Size() == 32 );
	}

	TEST_CASE( "[Array] Copy" )
	{
		dd::Array<int, 128> int_a;
		dd::Array<int, 128> int_b;

		for( int i = 0; i < 128; ++i )
		{
			int_a.Push( i );
		}

		int_b = int_a;

		REQUIRE( int_a.Size() == int_b.Size() );

		for( uint i = 0; i < int_a.Size(); ++i )
		{
			REQUIRE( int_a[ i ] == int_b[ i ] );
		}

		dd::Array<Test::NestedStruct, 128> complex_a;
		dd::Array<Test::NestedStruct, 128> complex_b;

		for( int i = 0; i < 128; ++i )
		{
			Test::NestedStruct s;
			s.SecondInt = i;
			complex_a.Push( s );
		}

		complex_b = complex_a;

		for( int i = 0; i < 128; ++i )
		{
			REQUIRE( complex_a[ i ].SecondInt == complex_b[ i ].SecondInt );
		}
	}
}