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
		String256 x = "testing the string serialization\ntesting the string serialization\ntesting the string serialization\ntesting the string serialization";
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
		s.Flt = 111.0f;
		s.Int = 222;
		s.Str = "LOL";
		s.Vec.Add( 1 );
		s.Vec.Add( 2 );
		s.Vec.Add( 3 );

		serializer.Serialize( s );
	}
}