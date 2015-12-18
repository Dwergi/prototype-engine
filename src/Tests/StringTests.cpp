//
// StringTests.h - Tests for dd::String.
// Copyright (C) Sebastian Nordgren 
// August 11th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "String_dd.h"
#include "SharedString.h"

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

	dd::String32 str3( "Test" );

	REQUIRE( str2 == str3 );
}

TEST_CASE( "[String] Assign" )
{
	dd::String32 str( "Test" );
	dd::String32 str2( str );

	REQUIRE( str == str2 );
	REQUIRE( str2 == "Test" );
	REQUIRE( str == "Test" );
}

TEST_CASE( "[String] Out of Scope" )
{
	dd::String32 outer;
	{
		dd::String32 inner( "Test" );

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

	REQUIRE( first == "foobarbaz" );

	first = "foo";

	REQUIRE( first == "foo" );

	dd::String8 second( "bar" );

	first += second;

	REQUIRE( first == "foobar" );
	REQUIRE( second == "bar" );
}

TEST_CASE( "[String] Grow" )
{
	dd::String8 start;

	for( int i = 0; i < 500; ++i )
	{
		start += "x";
	}
}

TEST_CASE( "[String] Find" )
{
	dd::String32 test( "A test string." );
	
	REQUIRE( test.Find( "elephant" ) == -1 );
	REQUIRE( test.Find( "test" ) == 2 );
	REQUIRE( test.Find( "A" ) == 0 );
	REQUIRE( test.Find( "." ) == test.Length() - 1 );
}

TEST_CASE( "[String] Substring" )
{
	dd::String32 test( "A test string." );

	dd::String32 res = test.Substring( 2, 4 );
	REQUIRE( res == "test" );
	
	res = test.Substring( 7, 6 );
	REQUIRE( res == "string" );

	res = test.Substring( 7 );
	REQUIRE( res == "string." );
}

TEST_CASE( "[String] Shrink To Fit" )
{
	dd::String8 start;
	start = "0123456789";

	// capacity has to have grown
	REQUIRE( start.Length() == 10 );

	start.ShrinkToFit();

	REQUIRE( start.Length() == 10 );
	REQUIRE( start.IsOnHeap() );
	REQUIRE( start == "0123456789" );

	start = "01234";
	start.ShrinkToFit();
	
	REQUIRE( start == "01234" );
	REQUIRE( !start.IsOnHeap() );
}

TEST_CASE( "[SharedString] Create" )
{
	const char* test = "A test string.";

	dd::SharedString str( test );
}

TEST_CASE( "[SharedString] Copy" )
{
	const char* test = "A test string.";

	dd::SharedString str( test );

	dd::SharedString str2( str );

	REQUIRE( str.c_str() == str2.c_str() );

	str.Clear();

	REQUIRE( str2 == test );
}

TEST_CASE( "[SharedString] Mixed Scopes" )
{
	const char* test = "A test string.";

	dd::SharedString str1( test );

	{
		dd::SharedString str2( test );

		{
			dd::SharedString str3( str2 );

			REQUIRE( str3.c_str() == str1.c_str() );
			REQUIRE( str3.c_str() == str2.c_str() );

			dd::SharedString str4 = str1;
			str3.Clear();

			REQUIRE( str4.c_str() == str2.c_str() );
		}

		dd::SharedString str5 = str1;

		REQUIRE( str2.c_str() == str1.c_str() );
	}

	REQUIRE( str1 == test );
}

TEST_CASE( "[SharedString] Usage Count" )
{
	const char* test = "A test string.";
	const char* test2 = "TheSecondTestString";

	dd::Vector<dd::SharedString> strings;

	SECTION( "Assignment" )
	{
		for( int i = 0; i < 64; ++i )
		{
			dd::SharedString& str = strings.Allocate();
			str = test;

			REQUIRE( str.UseCount() == i + 1 );
		}
	}
	
	dd::Vector<dd::SharedString> other_strings;

	SECTION( "Move Constructor" )
	{
		for( int i = 0; i < 64; ++i )
		{
			other_strings.Allocate( dd::SharedString( test2 ) );

			REQUIRE( other_strings[i].UseCount() == i + 1 );
		}

		dd::Vector<dd::SharedString> third_strings;

		SECTION( "Copy Constructor" )
		{
			for( int i = 0; i < 64; ++i )
			{
				dd::SharedString str( test2 );
				third_strings.Add( str );

				REQUIRE( str.UseCount() == 64 + i + 2 );
			}
		}
	}
}