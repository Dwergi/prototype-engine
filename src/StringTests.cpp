//
// StringTests.h - Tests for dd::String.
// Copyright (C) Sebastian Nordgren 
// August 11th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

TEST_CASE( "[String] Create" )
{
	dd::String str;
	str = "Test";

	REQUIRE( str == "Test" );

	REQUIRE( str[ 0 ] == 'T' );
	REQUIRE( str[ 1 ] == 'e' );
	REQUIRE( str[ 2 ] == 's' );
	REQUIRE( str[ 3 ] == 't' );

	dd::String str2( "Test" );

	REQUIRE( str == str2 );

	dd::String str3 = "Test";

	REQUIRE( str2 == str3 );
}

TEST_CASE( "[String] Assign" )
{
	dd::String str = "Test";
	dd::String str2 = str;

	REQUIRE( str == str2 );
	REQUIRE( str2 == "Test" );
	REQUIRE( str == "Test" );
}

TEST_CASE( "[String] Out of Scope" )
{
	dd::String outer;
	{
		dd::String inner = "Test";

		outer = inner;
	}
	
	REQUIRE( outer == "Test" );
}