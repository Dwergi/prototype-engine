//
// FunctionTests.h - Tests for Function.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

namespace Test
{
	struct TestClass
	{
		static bool sm_executed;
		static int sm_firstArgument;
		static float sm_secondArgument;

		void VoidNoArgs()
		{
			sm_executed = true;
		}

		void VoidSingleArg( int a )
		{
			sm_executed = true;
			sm_firstArgument = a;
		}

		void VoidTwoArgs( int a, float b )
		{
			sm_executed = true;
			sm_firstArgument = a;
			sm_secondArgument = b;
		}

		void VoidNoArgsConst() const
		{
			sm_executed = true;
		}

		void VoidSingleArgConst( int a ) const
		{
			sm_executed = true;
			sm_firstArgument = a;
		}

		void VoidTwoArgsConst( int a, float b ) const
		{
			sm_executed = true;
			sm_firstArgument = a;
			sm_secondArgument = b;
		}
		
		// return types

		int RetNoArgs()
		{
			sm_executed = true;
			return 1;
		}

		int RetSingleArg( int a )
		{
			sm_executed = true;
			sm_firstArgument = a;

			return 1;
		}

		int RetTwoArgs( int a, float b )
		{
			sm_executed = true;
			sm_firstArgument = a;
			sm_secondArgument = b;

			return 1;
		}

		int RetNoArgsConst() const
		{
			sm_executed = true;

			return 1;
		}

		int RetSingleArgConst( int a ) const
		{
			sm_executed = true;
			sm_firstArgument = a;

			return 1;
		}

		int RetTwoArgsConst( int a, float b ) const
		{
			sm_executed = true;
			sm_firstArgument = a;
			sm_secondArgument = b;

			return 1;
		}
	};
}

bool Test::TestClass::sm_executed = false;
int Test::TestClass::sm_firstArgument = -1;
float Test::TestClass::sm_secondArgument;

bool sm_executed = false;
int sm_firstArgument = -1;
float sm_secondArgument;

void VoidNoArgs()
{
	sm_executed = true;
}

void VoidSingleArg( int a )
{
	sm_executed = true;
	sm_firstArgument = a;
}

void VoidTwoArgs( int a, float b )
{
	sm_executed = true;
	sm_firstArgument = a;
	sm_secondArgument = b;
}

// return types

int RetNoArgs()
{
	sm_executed = true;
	return 1;
}

int RetSingleArg( int a )
{
	sm_executed = true;
	sm_firstArgument = a;

	return 1;
}

int RetTwoArgs( int a, float b )
{
	sm_executed = true;
	sm_firstArgument = a;
	sm_secondArgument = b;

	return 1;
}

TEST_CASE( "[Functions] Loose Functions" )
{
	sm_executed = false;
	sm_firstArgument = -1;
	sm_secondArgument = -1.0f;

	SECTION( "Void, No Args" )
	{
		dd::Function fn = DD_FUNCTION( VoidNoArgs );

		fn();

		REQUIRE( sm_executed );
	}

	SECTION( "Void, Single Arg" )
	{
		dd::Function fn = DD_FUNCTION( VoidSingleArg );

		fn( 1 );

		REQUIRE( sm_executed );
		REQUIRE( sm_firstArgument == 1 );
	}

	SECTION( "Void, Two Args" )
	{
		dd::Function fn = DD_FUNCTION( VoidTwoArgs );

		fn( 1, 10.0f );

		REQUIRE( sm_executed );
		REQUIRE( sm_firstArgument == 1 );
		REQUIRE( sm_secondArgument == 10.0f );
	}


	SECTION( "Return, No Args" )
	{
		dd::Function fn = DD_FUNCTION( RetNoArgs );

		int ret = -1;
		dd::Variable var( ret );
		fn( var );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( sm_executed );
	}

	SECTION( "Return, Single Arg" )
	{
		dd::Function fn = DD_FUNCTION( RetSingleArg );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1 );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( sm_executed );
		REQUIRE( sm_firstArgument == 1 );
	}

	SECTION( "Return, Two Args" )
	{
		dd::Function fn = DD_FUNCTION( RetTwoArgs );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1, 10.0f );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( sm_executed );
		REQUIRE( sm_firstArgument == 1 );
		REQUIRE( sm_secondArgument == 10.0f );
	}
}

TEST_CASE( "[Functions] Methods" )
{
	Test::TestClass::sm_executed = false;
	Test::TestClass::sm_firstArgument = -1;
	Test::TestClass::sm_secondArgument = -1.0f;

	Test::TestClass instance;

	SECTION( "Void, No Args, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidNoArgs );
		fn.Bind( instance );

		fn();

		REQUIRE( Test::TestClass::sm_executed );
	}

	SECTION( "Void, Single Arg, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidSingleArg );
		fn.Bind( instance );

		fn( 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
	}

	SECTION( "Void, Two Args, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidTwoArgs );
		fn.Bind( instance );

		fn( 1, 10.0f );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
		REQUIRE( Test::TestClass::sm_secondArgument == 10.0f );
	}

	SECTION( "Void, No Args, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidNoArgsConst );
		fn.Bind( instance );

		fn();

		REQUIRE( Test::TestClass::sm_executed );
	}

	SECTION( "Void, Single Arg, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidSingleArgConst );
		fn.Bind( instance );

		fn( 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
	}

	SECTION( "Void, Two Args, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::VoidTwoArgsConst );
		fn.Bind( instance );

		fn( 1, 10.0f );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
		REQUIRE( Test::TestClass::sm_secondArgument == 10.0f );
	}

	SECTION( "Return, No Args, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetNoArgs );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
	}

	SECTION( "Return, Single Arg, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetSingleArg );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1 );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
	}

	SECTION( "Return, Two Args, Not Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetTwoArgs );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1, 10.0f );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
		REQUIRE( Test::TestClass::sm_secondArgument == 10.0f );
	}

	SECTION( "Return, No Args, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetNoArgsConst );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
	}

	SECTION( "Return, Single Arg, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetSingleArgConst );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1 );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
	}

	SECTION( "Return, Two Args, Const" )
	{
		dd::Function fn = DD_FUNCTION( Test::TestClass::RetTwoArgsConst );
		fn.Bind( instance );

		int ret = -1;
		dd::Variable var( ret );
		fn( var, 1, 10.0f );

		REQUIRE( var.GetValue<int>() == 1 );

		REQUIRE( Test::TestClass::sm_executed );
		REQUIRE( Test::TestClass::sm_firstArgument == 1 );
		REQUIRE( Test::TestClass::sm_secondArgument == 10.0f );
	}
}