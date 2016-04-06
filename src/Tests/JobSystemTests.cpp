//
// JobSystemTests.cpp - Tests for the Job System.
// Copyright (C) Sebastian Nordgren 
// April 6th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "JobSystem.h"

static bool FreeExecuted;
static int FreeResult;

void FreeFunctionVoid()
{
	FreeExecuted = true;
}

void FreeFunctionWithArgs( int a, int b )
{
	FreeExecuted = true;
	FreeResult = a + b;
}

void FreeFunctionWithDifferentArgs( int a, char b )
{
	FreeExecuted = true;
	FreeResult = a + b;
}

struct TestStruct
{
	int Result;
	bool Executed;

	void TestFunctionVoid()
	{
		Executed = true;
	}

	void TestFunctionArgs( int a, int b )
	{
		Executed = true;
		Result = a + b;
	}
};

static bool ScheduledB = false;
static bool FinishedWaitingA = false;
static bool ContinueWaiting = true;
static bool CalledB = false;
static bool ExitedB = false;

void TestWaitB()
{
	CalledB = true;

	while( ContinueWaiting )
	{
		::Sleep( 0 );
	}

	ExitedB = true;
}

void TestWaitA( dd::JobSystem* system )
{
	system->Schedule( FUNCTION( TestWaitB ), "B" );

	ScheduledB = true;

	system->WaitForCategory( "B" );

	FinishedWaitingA = true;
}

TEST_CASE( "[JobSystem]" )
{
	dd::JobSystem system( 1 );

	FreeExecuted = false;

	SECTION( "Void Free Function" )
	{
		SECTION( "- Copy No Args" )
		{
			dd::Function fn = FUNCTION( FreeFunctionVoid );
			system.Schedule( fn, "Test" );
		}

		SECTION( "- Move No Args" )
		{
			system.Schedule( FUNCTION( FreeFunctionVoid ), "Test" );
		}

		SECTION( "- Copy Args" )
		{
			dd::Function fn = FUNCTION( FreeFunctionVoid );
			dd::FunctionArgs args;
			system.Schedule( fn, args, "Test" );
		}

		SECTION( "- Move Args" )
		{
			system.Schedule( FUNCTION( FreeFunctionVoid ), dd::FunctionArgs {}, "Test" );
		}

		system.WaitForCategory( "Test" );

		REQUIRE( FreeExecuted == true );
	} 

	SECTION( "Free Function With Args" )
	{
		FreeResult = 0;

		SECTION( "- Explicit FunctionArgs" )
		{
			dd::FunctionArgs args;
			args.AddArgument( dd::Variable( 1 ) );
			args.AddArgument( dd::Variable( 2 ) );
			system.Schedule( FUNCTION( FreeFunctionWithArgs ), args, "Test" );
		}

		SECTION( "- FunctionArgs::Create" )
		{
			system.Schedule( FUNCTION( FreeFunctionWithArgs ), dd::FunctionArgs::Create( 1, 2 ), "Test" );
		}

		SECTION( "- Different Args" )
		{
			system.Schedule( FUNCTION( FreeFunctionWithDifferentArgs ), dd::FunctionArgs::Create( -7, (char) 10 ), "Test" );
		}
		
		system.WaitForCategory( "Test" );

		REQUIRE( FreeExecuted == true );
		REQUIRE( FreeResult == 3 );
	}

	SECTION( "Method" )
	{
		TestStruct test_struct;
		test_struct.Executed = false;

		SECTION( "Void" )
		{
			SECTION( "- Explicit FunctionArgs" )
			{
				dd::FunctionArgs args;
				args.Context = dd::Variable( test_struct );
				system.Schedule( FUNCTION( TestStruct::TestFunctionVoid ), args, "Test" );
			}

			SECTION( "- FunctionArgs::Create" )
			{
				system.Schedule( FUNCTION( TestStruct::TestFunctionVoid ), dd::FunctionArgs::CreateMethod( &test_struct ), "Test" );
			}

			system.WaitForCategory( "Test" );

			REQUIRE( test_struct.Executed == true );
		}

		SECTION( "Args" )
		{
			test_struct.Result = 0;

			SECTION( "- Explicit FunctionArgs" )
			{
				dd::FunctionArgs args;
				args.Context = dd::Variable( test_struct );
				args.AddArgument( dd::Variable( 1 ) );
				args.AddArgument( dd::Variable( 2 ) );
				system.Schedule( FUNCTION( TestStruct::TestFunctionArgs ), args, "Test" );
			}

			SECTION( "- FunctionArgs::Create" )
			{
				system.Schedule( FUNCTION( TestStruct::TestFunctionArgs ), dd::FunctionArgs::CreateMethod( &test_struct, 1, 2 ), "Test" );
			}

			system.WaitForCategory( "Test" );

			REQUIRE( test_struct.Executed == true );
			REQUIRE( test_struct.Result == 3 );
		}
	}

	SECTION( "Waiting" )
	{
		REGISTER_TYPE( dd::JobSystem );

		ContinueWaiting = true;
		ScheduledB = false;
		FinishedWaitingA = false;
		CalledB = false;

		system.Schedule( FUNCTION( TestWaitA ), dd::FunctionArgs::Create( &system ), "A" );

		REQUIRE( ScheduledB );
		REQUIRE( CalledB );
		REQUIRE_FALSE( ExitedB );

		ContinueWaiting = false;

		system.WaitForCategory( "A" );

		REQUIRE( ExitedB );
		REQUIRE( FinishedWaitingA );
	}
}