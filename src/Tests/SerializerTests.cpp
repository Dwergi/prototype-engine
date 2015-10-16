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

TEST_CASE( "[Deserialization] POD types" )
{
	String256 in;

	SECTION( "Int" )
	{
		in = "125";

		int i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Float" )
	{
		in = "125.0";

		float i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125.0f );
	}

	SECTION( "Double" )
	{
		in = "125.0";

		double i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125.0 );
	}

	SECTION( "Uint" )
	{
		in = "125";

		uint i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Char" )
	{
		in = "c";

		char i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 'c' );
	}

	SECTION( "Byte" )
	{
		in = "125";

		byte i = 0;
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Uint16" )
	{
		in = "125";
		uint16 i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Uint64" )
	{
		in = "125";
		uint64 i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Uint64" )
	{
		in = "\"125\"";
		char c[256];

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( &c[0] );

		REQUIRE( strcmp( c, "125" ) == 0 );
	}

	SECTION( "Int64" )
	{
		in = "125";
		int64 i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Negative" )
	{
		in = "-125";
		int i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == -125 );
	}

	SECTION( "Zero" )
	{
		in = "0";
		int i = 1111;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == 0 );
	}

	SECTION( "Negative float" )
	{
		in = "-125.0";

		float i = 0;

		JSONDeserializer deserializer( in );
		deserializer.Deserialize( i );

		REQUIRE( i == -125.0f );
	}
}

TEST_CASE( "[Deserialization] Structs from JSON" )
{
	REGISTER_TYPE( Test::SimpleStruct );
	REGISTER_TYPE( Test::NestedStruct );

	String256 in;

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

		Test::SimpleStruct result;
		
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( result );

		REQUIRE( result.Int == 0 );
		REQUIRE( result.Str == "LOL" );
		REQUIRE( result.Flt == 111.0f );
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