//
// StringTests.h - Tests for dd::String.
// Copyright (C) Sebastian Nordgren 
// August 11th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

TEST_CASE( "[String] Create" )
{
	dd::String32 str;
	str = "Test";

	REQUIRE( str == "Test" );

	REQUIRE( str[ 0 ] == 'T' );
	REQUIRE( str[ 1 ] == 'e' );
	REQUIRE( str[ 2 ] == 's' );
	REQUIRE( str[ 3 ] == 't' );

	dd::String32 str2( "Test" );

	REQUIRE( str == str2 );

	dd::String32 str3 = "Test";

	REQUIRE( str2 == str3 );
}

TEST_CASE( "[String] Assign" )
{
	dd::String32 str = "Test";
	dd::String32 str2 = str;

	REQUIRE( str == str2 );
	REQUIRE( str2 == "Test" );
	REQUIRE( str == "Test" );
}

TEST_CASE( "[String] Out of Scope" )
{
	dd::String32 outer;
	{
		dd::String32 inner = "Test";

		outer = inner;
	}
	
	REQUIRE( outer == "Test" );
}

TEST_CASE( "[String] Concatenation" )
{
	dd::String8 first( "foo" );

	first += "bar";

	REQUIRE( first == "foobar" );

	first += "baz";

	REQUIRE( first == "foorbarbaz" );

	first = "foo";

	REQUIRE( first == "foo" );

	dd::String8 second( "bar" );

	first += second;

	REQUIRE( first == "foobar" );
	REQUIRE( second == "bar" );
}