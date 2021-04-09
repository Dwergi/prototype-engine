//
// PropertyListTests.cpp - Tests for PropertyList.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PCH.h"
#include "Tests.h"

#include "PropertyList.h"
#include "TransformComponent.h"
#include "Recorder.h"

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

DD_CLASS_CPP( Test::SimpleStruct );
DD_CLASS_CPP( Test::NestedStruct );

TEST_CASE( "[Property] Integer properties" )
{
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
