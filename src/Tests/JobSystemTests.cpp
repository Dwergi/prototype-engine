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

static volatile bool ScheduledB = false;
static volatile bool FinishedWaitingA = false;
static volatile bool ContinueWaiting = true;
static volatile bool CalledB = false;
static volatile bool ExitedB = false;

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
			auto f = system.Schedule( FUNCTION( FreeFunctionVoid ) );
			f.wait();
		}

		SECTION( "- std::function" )
		{
			auto f = system.Schedule( &FreeFunctionVoid );
			f.wait();
		}

		REQUIRE( FreeExecuted == true );
	} 

	SECTION( "Free Function With Args" )
	{
		FreeResult = 0;

		auto f = system.Schedule( std::bind( &FreeFunctionWithDifferentArgs, -7, (char) 10 ) );
		f.wait();
		
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
				auto f = system.Schedule( fn );
				f.wait();
			}

			SECTION( "- std::function" )
			{
				auto f = system.Schedule( std::bind( &TestStruct::TestFunctionVoid, &test_struct ) );
				f.wait();
			}

			REQUIRE( test_struct.Executed == true );
		}

		SECTION( "Args" )
		{
			test_struct.Result = 0;

			auto f = system.Schedule( std::bind( &TestStruct::TestFunctionArgs, &test_struct, 1, 2 ), "Test" );
			f.wait();

			REQUIRE( test_struct.Executed == true );
			REQUIRE( test_struct.Result == 3 );
		}
	}
}