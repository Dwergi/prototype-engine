//
// StackArrayTests.h - Tests for Array.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "Tests.h"

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
	}

	TEST_CASE( "[Array] Struct" )
	{
		dd::Array<Test::NestedStruct, 32> container;
	
		DD_ASSERT( sizeof( Test::NestedStruct ) > sizeof( Test::SimpleStruct ) );
		DD_ASSERT( sizeof( dd::Array<Test::NestedStruct, 32> ) > sizeof( Test::NestedStruct ) * 32 );

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
		dd::Array<int, 16> int_a;
		dd::Array<int, 16> int_b;

		for( int i = 0; i < 16; ++i )
		{
			int_a.Push( i );
		}

		int_b = int_a;

		REQUIRE( int_a.Size() == int_b.Size() );

		for( int i = 0; i < int_a.Size(); ++i )
		{
			REQUIRE( int_a[ i ] == int_b[ i ] );
		}

		dd::Array<Test::NestedStruct, 16> complex_a;
		dd::Array<Test::NestedStruct, 16> complex_b;

		for( int i = 0; i < 16; ++i )
		{
			Test::NestedStruct s;
			s.SecondInt = i;
			complex_a.Push( s );
		}

		complex_b = complex_a;

		for( int i = 0; i < 16; ++i )
		{
			REQUIRE( complex_a[ i ].SecondInt == complex_b[ i ].SecondInt );
		}
	}
}