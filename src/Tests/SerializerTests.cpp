//
// SerializerTests.cpp - Tests for serialization.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "JSONSerializer.h"

#include "TestTypes.h"

using namespace dd;
/*
TEST_CASE( "[Serialization] Serialize to JSON" )
{
	DD_REGISTER_CLASS( Test::SimpleStruct );
	DD_REGISTER_CLASS( Test::NestedStruct );

	String256 out;
	WriteStream stream( out );
	JSONSerializer serializer( stream );

	SECTION( "Int" )
	{
		int x = 5;
		bool success = serializer.Serialize( x );
		REQUIRE( success == true );
		REQUIRE( stream.Offset() == 0 );
	}

	SECTION( "Float" )
	{
		float x = 5.0f;
		bool success = serializer.Serialize( x );

		REQUIRE( success == true );
		REQUIRE( out.StartsWith( "5" ) );
	}

	SECTION( "String" )
	{
		String8 x( "test" );
		bool success = serializer.Serialize( x );

		REQUIRE( success == true );

		// strings should have extra quotes around them
		REQUIRE( out == "\"test\"" );
	}

	SECTION( "Long String" )
	{
		String256 x( "testing the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n" );

		bool success = serializer.Serialize( x );
		
		String256 y;
		y += "\"";
		y += x;
		y += "\"";

		REQUIRE( success == true );
		//REQUIRE( out == y );
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
		bool success = serializer.Serialize( simple );
		REQUIRE( success );

		const char* result =
			"{\n"
			"	\"type\": \"Test::SimpleStruct\",\n"
			"	\"members\": {\n"
			"		\"Int\": 222,\n"
			"		\"Str\": \"LOL\",\n"
			"		\"Flt\": 111.00,\n"
			"		\"Vec\": [\n"
			"			1,\n"
			"			2,\n"
			"			3\n"
			"		]\n"
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
			"	\"type\": \"Test::NestedStruct\",\n"
			"	\"members\": {\n"
			"		\"Nested\": {\n"
			"			\"type\": \"Test::SimpleStruct\",\n"
			"			\"members\": {\n"
			"				\"Int\": 222,\n"
			"				\"Str\": \"LOL\",\n"
			"				\"Flt\": 111.00,\n"
			"				\"Vec\": [\n"
			"					1,\n"
			"					2,\n"
			"					3\n"
			"				]\n"
			"			}\n"
			"		},\n"
			"		\"SecondInt\": 333\n"
			"	}\n"
			"}";

		REQUIRE( out == nested_result );
	}
}

TEST_CASE( "[Deserialization] POD From JSON" )
{
	char in[2048];
	ReadStream stream( in, 2048 );

	SECTION( "Int" )
	{
		strcpy_s( in, "125" );

		int i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "Float" )
	{
		strcpy_s( in, "125.0" );

		float i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125.0f );
	}

	SECTION( "Double" )
	{
		strcpy_s( in, "125.0" );

		double i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125.0 );
	}

	SECTION( "Uint" )
	{
		strcpy_s( in, "125" );

		uint i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "Byte" )
	{
		strcpy_s( in, "125" );

		byte i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "Uint16" )
	{
		strcpy_s( in, "125" );
		uint16 i = 0;

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "Uint64" )
	{
		strcpy_s( in, "125" );
		uint64 i = 0;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "String" )
	{
		strcpy_s( in, "\"125\"" );

		JSONDeserializer deserializer( stream );

		String256 c;
		Variable v( c );
		bool success = deserializer.Deserialize( v );

		REQUIRE( success );
		REQUIRE( c == "125" );
	}

	SECTION( "SharedString" )
	{
		strcpy_s( in, "\"125\"" );

		JSONDeserializer deserializer( stream );

		SharedString c;
		Variable v( c );
		bool success = deserializer.Deserialize( v );

		REQUIRE( success );
		REQUIRE( c == "125" );
	}

	SECTION( "Int64" )
	{
		strcpy_s( in, "125" );
		int64 i = 0;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 125 );
	}

	SECTION( "Negative" )
	{
		strcpy_s( in, "-125" );
		int i = 0;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == -125 );
	}

	SECTION( "Zero" )
	{
		strcpy_s( in, "0" );
		int i = 1111;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == 0 );
	}

	SECTION( "Negative float" )
	{
		strcpy_s( in, "-125.0" );
		float i = 0;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( i );

		REQUIRE( success );
		REQUIRE( i == -125.0f );
	}
}

TEST_CASE( "[Deserialization] Structs from JSON" )
{
	DD_REGISTER_CLASS( Test::SimpleStruct );
	DD_REGISTER_CLASS( Test::NestedStruct );

	byte in[2048];
	ReadStream stream( in, 2048 );

	Test::SimpleStruct s;
	s.Int = 0;
	s.Str = "LOL";
	s.Flt = 111.0f;
	s.Vec.Add( 1 );
	s.Vec.Add( 2 );
	s.Vec.Add( 3 );

	SECTION( "Struct" )
	{
		WriteStream out_stream( in, 2048 );
		JSONSerializer serializer( out_stream );
		serializer.Serialize( s );

		Test::SimpleStruct result;
		
		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( result );

		REQUIRE( success );

		REQUIRE( result.Int == 0 );
		REQUIRE( result.Str == "LOL" );
		REQUIRE( result.Flt == 111.0f );

		REQUIRE( result.Vec[0] == 1 );
		REQUIRE( result.Vec[1] == 2 );
		REQUIRE( result.Vec[2] == 3 );
	}


	SECTION( "Nested Struct" )
	{
		Test::NestedStruct complex;
		complex.SecondInt = 50;
		complex.Nested = s;

		WriteStream out_stream( in, 2048 );
		JSONSerializer serializer( out_stream );
		serializer.Serialize( complex );

		JSONDeserializer deserializer( stream );
		bool success = deserializer.Deserialize( complex );

		REQUIRE( success );

		REQUIRE( complex.Nested == s );
		REQUIRE( complex.SecondInt == 50 );
	}
}*/