//
// SerializerTests.cpp - Tests for serialization.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "JSONSerializer.h"

#include "TestTypes.h"

using namespace dd;

TEST_CASE( "[Serialization] Serialize to JSON" )
{
	RegisterDefaultTypes();
	REGISTER_TYPE( Test::SimpleStruct );
	REGISTER_TYPE( Test::NestedStruct );

	String256 out;

	JSONSerializer serializer( out );

	SECTION( "Int" )
	{
		int x = 5;
		serializer.Serialize( x );
		REQUIRE( out == "5" );
	}

	SECTION( "Float" )
	{
		float x = 5.0f;
		serializer.Serialize( x );
		REQUIRE( out.StartsWith( "5.0" ) );
	}

	SECTION( "String" )
	{
		String8 x = "test";
		serializer.Serialize( x );

		// strings should have extra quotes around them
		REQUIRE( out == "\"test\"" );
	}

	SECTION( "Long String" )
	{
		String256 x = "testing the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n";

		serializer.Serialize( x );
		
		String256 y;
		y += "\"";
		y += x;
		y += "\"";

		REQUIRE( out == y );
	}

	Test::SimpleStruct simple;
	simple.Int = 222;
	simple.Str = "LOL";
	simple.Flt = 111.0f;
	simple.Vec.Add( 1 );
	simple.Vec.Add( 2 );
	simple.Vec.Add( 3 );

	SECTION( "Struct" )
	{
		serializer.Serialize( simple );

		const char* result =
			"{\n"
			"	\"type\" : \"Test::SimpleStruct\",\n"
			"	\"members\" : \n"
			"	{\n"
			"		\"Int\" : 222,\n"
			"		\"Str\" : \"LOL\",\n"
			"		\"Flt\" : 111.000000,\n"
			"		\"Vec\" : [1,2,3]\n"
			"	}\n"
			"}";

		REQUIRE( out == result );
	}

	SECTION( "Nested Struct" )
	{
		Test::NestedStruct nested;
		nested.SecondInt = 333;
		nested.Nested = simple;

		serializer.Serialize( nested );

		const char* nested_result =
			"{\n"
			"	\"type\" : \"Test::NestedStruct\",\n"
			"	\"members\" : \n"
			"	{\n"
			"		\"Nested\" : \n"
			"		{\n"
			"			\"type\" : \"Test::SimpleStruct\",\n"
			"			\"members\" : \n"
			"			{\n"
			"				\"Int\" : 222,\n"
			"				\"Str\" : \"LOL\",\n"
			"				\"Flt\" : 111.000000,\n"
			"				\"Vec\" : [1,2,3]\n"
			"			}\n"
			"		},\n"
			"		\"SecondInt\" : 333\n"
			"	}\n"
			"}";

		REQUIRE( out == nested_result );
	}
}

TEST_CASE( "[Serialization] Deserialize from JSON" )
{
	RegisterDefaultTypes();
	REGISTER_TYPE( Test::SimpleStruct );
	REGISTER_TYPE( Test::NestedStruct );

	String256 in;

	SECTION( "Int" )
	{
		in = "125";

		int i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );
	}

	Test::SimpleStruct s;
	s.Int = 0;
	s.Str = "LOL";
	s.Flt = 111.0f;
	s.Vec.Add( 1 );
	s.Vec.Add( 2 );
	s.Vec.Add( 3 );

	SECTION( "Struct" )
	{
		JSONSerializer serializer( in );
		serializer.Serialize( s );
		
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( s );

		REQUIRE( s.Int == 0 );
		REQUIRE( s.Str == "LOL" );
		REQUIRE( s.Flt == 111.0f );
	}


	SECTION( "Nested Struct" )
	{
		Test::NestedStruct complex;
		complex.SecondInt = 50;
		complex.Nested = s;

		JSONSerializer serializer( in );
		serializer.Serialize( complex );

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( complex );

		REQUIRE( complex.Nested == s );
		REQUIRE( complex.SecondInt == 50 );
	}
}