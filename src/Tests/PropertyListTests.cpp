//
// PropertyListTests.cpp - Tests for PropertyList.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"
/*

#include "PropertyList.h"
#include "TransformComponent.h"
#include "Recorder.h"
#include "Vector4.h"

struct SimpleStruct
{
	int Int;
	dd::String Str;
	float Flt;
	dd::Vector<int> Vec;

	void Double();
	int Multiply( int x );

	BEGIN_MEMBERS( SimpleStruct )
		MEMBER( SimpleStruct, int, Int, "Integer" );
		MEMBER( SimpleStruct, dd::String, Str, "String" );
		MEMBER( SimpleStruct, float, Flt, "Float" );
		METHOD( SimpleStruct, Double, void, void );
		METHOD( SimpleStruct, Multiply, int, int );
	END_MEMBERS
};

void SimpleStruct::Double()
{

}

int SimpleStruct::Multiply( int x )
{
	return x;
}

struct NestedStruct
{
	SimpleStruct Struct;
	int SecondInt;

	BEGIN_MEMBERS( NestedStruct )
		MEMBER( NestedStruct, SimpleStruct, Struct, "Struct" );
		MEMBER( NestedStruct, int, SecondInt, "Second Integer" );
	END_MEMBERS
};

void test_int_prop( dd::Property* int_prop )
{
	int i = 5;
	int_prop->Set( i );

	i = 0;
	int_prop->Get( i );
	REQUIRE( i == 5 );

	i = 0;
	int_prop->Get( i );
	REQUIRE( i == 5 );

	i = 20;
	int_prop->Set( i );
	int_prop->Get( i );
	REQUIRE( i == 20 );
}

TEST_CASE( "[property] Integer properties" )
{
	SimpleStruct::RegisterType();
	SimpleStruct test_struct;

	SECTION( "Simple" )
	{
		dd::PropertyList<SimpleStruct> props( test_struct );
		dd::Property* int_prop = props.Find( "Integer" );

		test_int_prop( int_prop );

		REQUIRE( test_struct.Int == 20 );
	}

	SECTION( "Nested Struct ")
	{
		NestedStruct::RegisterType();
		NestedStruct nested;
		dd::PropertyList<NestedStruct> nested_props( nested );

		SECTION( "Nested" )
		{
			dd::Property* nested_int = nested_props.Find( "Integer" );
			test_int_prop( nested_int );

			REQUIRE( nested.Struct.Int == 20 );
		}
		SECTION( "Offset" )
		{
			dd::Property* second_int = nested_props.Find( "SecondInt" );
			test_int_prop( second_int );

			REQUIRE( nested.SecondInt == 20 );
		}
	}
}

TEST_CASE( "[property] Recorder" )
{
	dd::Vector4::RegisterType();
	dd::TransformComponent::RegisterType();

	dd::TransformComponent cmp;
	cmp.Position.X = 500;

	dd::PropertyList<dd::TransformComponent> transform_list( cmp );
	dd::Recorder<float> x_recorder( transform_list.Find( "X" ) );

	float value = 0;

	value = x_recorder;
	REQUIRE( value == 500.f );

	x_recorder = 200.f;
	value = x_recorder;
	REQUIRE( value == 200.f );

	x_recorder.Undo();
	REQUIRE( x_recorder == 500.f );
}*/