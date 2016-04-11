//
// PropertyListTests.cpp - Tests for PropertyList.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "PropertyList.h"
#include "TransformComponent.h"
#include "Recorder.h"
#include "Vector4.h"

#include "TestTypes.h"

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
	REGISTER_TYPE( Test::SimpleStruct );
	Test::SimpleStruct test_struct;

	SECTION( "Simple" )
	{
		dd::PropertyList props( test_struct );
		dd::Property* int_prop = props.Find( "Int" );

		REQUIRE( int_prop != nullptr );

		test_int_prop( int_prop );

		REQUIRE( test_struct.Int == 20 );
	}

	SECTION( "Nested Struct ")
	{
		REGISTER_TYPE( Test::NestedStruct );
		Test::NestedStruct nested;
		dd::PropertyList nested_props( nested );

		SECTION( "Nested" )
		{
			dd::Property* nested_int = nested_props.Find( "Int" );

			REQUIRE( nested_int != nullptr );

			test_int_prop( nested_int );

			REQUIRE( nested.Nested.Int == 20 );
		}
		SECTION( "Offset" )
		{
			dd::Property* second_int = nested_props.Find( "SecondInt" );

			REQUIRE( second_int != nullptr );

			test_int_prop( second_int );

			REQUIRE( nested.SecondInt == 20 );
		}
	}
}

TEST_CASE( "[property] Recorder" )
{
	REGISTER_TYPE( dd::EntityHandle );
	REGISTER_TYPE( dd::Component );
	REGISTER_TYPE( dd::Vector4 );
	REGISTER_TYPE( dd::TransformComponent );

	dd::TransformComponent cmp;
	cmp.Position.x = 500;

	dd::PropertyList transform_list( cmp );
	dd::Recorder<float> x_recorder( transform_list.Find( "x" ) );

	float value = 0;

	value = x_recorder;
	REQUIRE( value == 500.f );

	x_recorder = 200.f;
	value = x_recorder;
	REQUIRE( value == 200.f );

	x_recorder.Undo();
	REQUIRE( x_recorder == 500.f );
}