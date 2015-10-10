//
// SerializerTests.cpp - Tests for serialization.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "JSONSerializer.h"

using namespace dd;

struct SimpleStruct
{
	int Int;
	dd::String32 Str;
	float Flt;
	dd::Vector<int> Vec;

	void Double();
	int Multiply( int x );

	BEGIN_MEMBERS( SimpleStruct )
		MEMBER( Int );
		MEMBER( Str );
		MEMBER( Flt );
		MEMBER( Vec );
		METHOD( Multiply );
	END_MEMBERS
};

TEST_CASE( "[Serialization] Serialize to JSON" )
{
	RegisterDefaultTypes();

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

	SECTION( "Struct" )
	{
		REGISTER_TYPE( SimpleStruct );

		SimpleStruct s;
		s.Int = 222;
		s.Str = "LOL";
		s.Flt = 111.0f;
		s.Vec.Add( 1 );
		s.Vec.Add( 2 );
		s.Vec.Add( 3 );

		serializer.Serialize( s );

		const char* result = 
		"{\n"
		"	\"type\" : \"SimpleStruct\",\n"
		"	\"members\" : \n"
		"	{\n"
		"		\"Int\" : 222,\n"
		"		\"Str\" : \"LOL\",\n"
		"		\"Flt\" : 111.000000,\n"
		"		\"Vec\" : [1,2,3]\n"
		"	}\n"
		"}\n";

		REQUIRE( out == result );
	}
}

TEST_CASE( "[Serialization] Deserialize from JSON" )
{
	RegisterDefaultTypes();

	String256 in;

	SECTION( "Int" )
	{
		in = "125";
		JSONDeserializer deserializer( in );
		
		int i = 0;
		deserializer.Deserialize( i );
	}

	SECTION( "Struct" )
	{
		SimpleStruct s;
		s.Int = 0;
		s.Str = "LOL";
		s.Flt = 111.0f;

		JSONSerializer serializer( in );
		serializer.Serialize( s );
		
		JSONDeserializer deserializer( in );
		deserializer.Deserialize( s );

		REQUIRE( s.Int == 0 );
		REQUIRE( s.Str == "LOL" );
		REQUIRE( s.Flt == 111.0f );
	}
}