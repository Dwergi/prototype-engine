//
// JobSystemTests.cpp - Tests for the Job System.
// Copyright (C) Sebastian Nordgren 
// April 6th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "JobSystem.h"
#include "Timer.h"

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
		SECTION( "- Function" )
		{
			system.Schedule( FUNCTION( FreeFunctionVoid ), "Test" );
		}

		SECTION( "- std::function" )
		{
			system.Schedule( &FreeFunctionVoid, "Test" );
		}

		system.WaitForCategory( "Test" );

		REQUIRE( FreeExecuted == true );
	} 

	SECTION( "Free Function With Args" )
	{
		FreeResult = 0;

		system.Schedule( std::bind( &FreeFunctionWithDifferentArgs, -7, (char) 10 ), "Test" );
		
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
			SECTION( "- Function" )
			{
				dd::Function fn = FUNCTION( TestStruct::TestFunctionVoid );
				fn.Bind( test_struct );
				system.Schedule( fn, "Test" );
			}

			SECTION( "- std::function" )
			{
				system.Schedule( std::bind( &TestStruct::TestFunctionVoid, &test_struct ), "Test" );
			}

			system.WaitForCategory( "Test" );

			REQUIRE( test_struct.Executed == true );
		}

		SECTION( "Args" )
		{
			test_struct.Result = 0;

			system.Schedule( std::bind( &TestStruct::TestFunctionArgs, &test_struct, 1, 2 ), "Test" );

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

		system.Schedule( std::bind( &TestWaitA, &system ), "A" );

		// this should only take some arbitrarily short amount of time, let's say 0.1 seconds
		dd::Timer t;
		t.Start();
		while( !ScheduledB && t.Time() < 0.1f ) { ::Sleep( 5 ); }

		REQUIRE( ScheduledB );
		REQUIRE( CalledB );
		REQUIRE_FALSE( ExitedB );

		ContinueWaiting = false;

		system.WaitForCategory( "A" );

		REQUIRE( ExitedB );
		REQUIRE( FinishedWaitingA );
	}
}