//
// BinarySerializerTests.cpp - Tests for serialization.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "BinarySerializer.h"

#include "TestTypes.h"

using namespace dd;

TEST_CASE( "[Serialization] Serialize to Binary" )
{
	REGISTER_TYPE( Test::SimpleStruct );
	REGISTER_TYPE( Test::NestedStruct );

	byte out[2048];
	WriteStream stream( &out[0], 2048 );
	BinarySerializer serializer( stream );

	SECTION( "Int" )
	{
		int x = 5;
		serializer.Serialize( x );

		ReadStream res( &out[0], 2048 );

		int cmp = res.ReadPOD<int>();
		REQUIRE( cmp == 5 );
	}

	SECTION( "Float" )
	{
		float x = 5.0f;
		serializer.Serialize( x );

		ReadStream res( &out[0], 2048 );

		float cmp = res.ReadPOD<float>();
		REQUIRE( cmp == 5.0f );
	}

	SECTION( "String" )
	{
		String8 x( "test" );
		serializer.Serialize( x );

		REQUIRE( stream.Offset() == x.Length() + 1 ); // includes null termination

		char* cmp = reinterpret_cast<char*>(&out[0]);
		REQUIRE( x == cmp );
	}

	SECTION( "Long String" )
	{
		String256 x( "testing the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n"
			"testing the string serialization\ntesting the string serialization\ntesting the string serialization\n" );

		serializer.Serialize( x );

		REQUIRE( stream.Offset() == x.Length() + 1 ); // includes null termination

		char* cmp = reinterpret_cast<char*>(&out[0]);
		REQUIRE( x == cmp );
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

		ReadStream res( &out[0], 2048 );

		BitField bitfield( 4 );
		bitfield.Read( res );

		// top 4 bits are set
		REQUIRE( bitfield.Get( 0 ) );
		REQUIRE( bitfield.Get( 1 ) );
		REQUIRE( bitfield.Get( 2 ) );
		REQUIRE( bitfield.Get( 3 ) );

		int integer = res.ReadPOD<int>();
		REQUIRE( simple.Int == integer );

		String8 str;
		res.Read( str );
		REQUIRE( simple.Str == str );

		float flt = res.ReadPOD<float>();
		REQUIRE( flt == simple.Flt );

		uint size = res.ReadPOD<uint>();
		REQUIRE( simple.Vec.Size() == size );

		int elem1 = res.ReadPOD<int>();
		REQUIRE( elem1 == simple.Vec[0] );

		int elem2 = res.ReadPOD<int>();
		REQUIRE( elem2 == simple.Vec[1] );

		int elem3 = res.ReadPOD<int>();
		REQUIRE( elem3 == simple.Vec[2] );
	}

	SECTION( "Nested Struct" )
	{
		Test::NestedStruct nested;
		nested.Nested = simple;
		nested.SecondInt = 333;

		serializer.Serialize( nested );

		ReadStream res( &out[0], 2048 );

		BitField nestedBitfield( 3 );
		nestedBitfield.Read( res );

		// top 3 bits are set
		REQUIRE( nestedBitfield.Get( 0 ) );
		REQUIRE( nestedBitfield.Get( 1 ) );

		BitField bitfield( 4 );
		bitfield.Read( res );

		// top 4 bits are set
		REQUIRE( bitfield.Get( 0 ) );
		REQUIRE( bitfield.Get( 1 ) );
		REQUIRE( bitfield.Get( 2 ) );
		REQUIRE( bitfield.Get( 3 ) );

		int integer = res.ReadPOD<int>();
		REQUIRE( simple.Int == integer );

		String8 str;
		res.Read( str );
		REQUIRE( simple.Str == str );

		float flt = res.ReadPOD<float>();
		REQUIRE( flt == simple.Flt );

		uint size = res.ReadPOD<uint>();
		REQUIRE( simple.Vec.Size() == size );

		int elem1 = res.ReadPOD<int>();
		REQUIRE( elem1 == simple.Vec[0] );

		int elem2 = res.ReadPOD<int>();
		REQUIRE( elem2 == simple.Vec[1] );

		int elem3 = res.ReadPOD<int>();
		REQUIRE( elem3 == simple.Vec[2] );

		int secondInt = res.ReadPOD<int>();
		REQUIRE( nested.SecondInt == secondInt );
	}
}

TEST_CASE( "[Deserialization] POD types" )
{
	byte in[2048];
	ReadStream stream( in, 2048 );

	SECTION( "Int" )
	{
		*(int*) in = 0x12345678;

		int i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0x12345678 );
	}

	SECTION( "Float" )
	{
		*(float*) in = 125.0f;

		float i = 0;
		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 125.0f );
	}

	SECTION( "Double" )
	{
		*(double*) in = 125.0;

		double i = 0;
		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 125.0 );
	}

	SECTION( "Uint" )
	{
		*(uint*) in = 0x12345678;

		uint i = 0;
		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0x12345678 );
	}

	SECTION( "Char" )
	{
		*(char*) in = 'c';

		char i = 0;
		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 'c' );
	}

	SECTION( "Byte" )
	{
		*(byte*) in = 125;

		byte i = 0;
		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 125 );
	}

	SECTION( "Uint16" )
	{
		*(uint16*) in = 0x1234;
		uint16 i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0x1234 );
	}

	SECTION( "Uint64" )
	{
		*(uint64*) in = 0x1234567890abcdef;
		uint64 i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0x1234567890abcdef );
	}

	SECTION( "char*" )
	{
		in[0] = '1';
		in[1] = '2';
		in[2] = '5';
		in[3] = '\0';

		BinaryDeserializer deserializer( stream );

		char c[256];
		Variable v( c );
		deserializer.Deserialize( v );

		REQUIRE( strcmp( c, "125" ) == 0 );
	}

	SECTION( "Int64" )
	{
		*(int64*) in = 0x1234567890abcdef;
		int64 i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0x1234567890abcdef );
	}

	SECTION( "Negative" )
	{
		*(int*) in = -125;
		int i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == -125 );
	}

	SECTION( "Zero" )
	{
		*(int*) in = 0;
		int i = 1111;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == 0 );
	}

	SECTION( "Negative float" )
	{
		*(float*) in = -125.0;

		float i = 0;

		BinaryDeserializer deserializer( stream );
		deserializer.Deserialize( i );

		REQUIRE( i == -125.0f );
	}
}

TEST_CASE( "[Deserialization] Structs from Binary" )
{
	REGISTER_TYPE( Test::SimpleStruct );
	REGISTER_TYPE( Test::NestedStruct );

	byte in[2048];
	WriteStream writer( in, 2048 );
	ReadStream reader( in, 2048 );

	Test::SimpleStruct s;
	s.Int = 0;
	s.Str = "LOL";
	s.Flt = 111.0f;
	s.Vec.Add( 1 );
	s.Vec.Add( 2 );
	s.Vec.Add( 3 );

	SECTION( "Struct" )
	{
		BinarySerializer serializer( writer );
		serializer.Serialize( s );

		Test::SimpleStruct result;

		BinaryDeserializer deserializer( reader );
		deserializer.Deserialize( result );

		REQUIRE( result.Int == 0 );
		REQUIRE( result.Str == "LOL" );
		REQUIRE( result.Flt == 111.0f );
		REQUIRE( result.Vec[ 0 ] == 1 );
		REQUIRE( result.Vec[ 1 ] == 2 );
		REQUIRE( result.Vec[ 2 ] == 3 );
	}


	SECTION( "Nested Struct" )
	{
		Test::NestedStruct complex;
		complex.SecondInt = 50;
		complex.Nested = s;

		BinarySerializer serializer( writer );
		serializer.Serialize( complex );

		BinaryDeserializer deserializer( reader );
		deserializer.Deserialize( complex );

		REQUIRE( complex.Nested == s );
		REQUIRE( complex.SecondInt == 50 );
	}
}
