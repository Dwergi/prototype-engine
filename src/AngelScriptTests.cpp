//
// AngelScriptTests.cpp - Tests for the AngelScript bindings.
// Copyright (C) Sebastian Nordgren 
// June 1st 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "AngelScriptEngine.h"

struct AngelScriptTest
{
	int Integer;
	bool Boolean;

	AngelScriptTest()
	{
	}

	AngelScriptTest( int integer, bool boolean )
		: Integer { integer }
		, Boolean { boolean }
	{
	}

	void TestFunction()
	{
		Integer = 1;
		FunctionCalled = true;
	}

	int TestFunctionRet()
	{
		Boolean = true;
		FunctionRetCalled = true;
		return 1;
	}

	void TestFunctionArg( int a, int b )
	{
		Integer = a + b;
		FunctionArgCalled = true;
	}

	int TestGetInt() const
	{
		return Integer;
	}

	static bool FunctionCalled;
	static bool FunctionRetCalled;
	static bool FunctionArgCalled;

	BEGIN_SCRIPT_OBJECT( AngelScriptTest )
	END_TYPE
};

bool AngelScriptTest::FunctionCalled = false;
bool AngelScriptTest::FunctionRetCalled = false;
bool AngelScriptTest::FunctionArgCalled = false;

namespace Test
{
	namespace AngelScript
	{
		extern uint s_counter;
		uint s_counter = 0;

		int FreeFunction()
		{
			return 5;
		}

		int FreeFunctionWithArg( int a )
		{
			return a * 5;
		}
	}
}

TEST_CASE( "[AngelScript] Register Class" )
{
	REGISTER_TYPE( AngelScriptTest );

	dd::AngelScriptEngine engine;
	engine.RegisterType<AngelScriptTest, false>();

	SECTION( "Members" )
	{
		engine.RegisterMember<AngelScriptTest, int, &AngelScriptTest::Integer>( "Integer", GET_TYPE( AngelScriptTest ) );
		engine.RegisterMember<AngelScriptTest, bool, &AngelScriptTest::Boolean>( "Boolean", GET_TYPE( AngelScriptTest ) );
	}

	SECTION( "Methods" )
	{
		engine.RegisterFunction<decltype(&AngelScriptTest::TestFunction), &AngelScriptTest::TestFunction>( "TestFunction" );
		engine.RegisterFunction<decltype(&AngelScriptTest::TestFunctionArg), &AngelScriptTest::TestFunctionArg>( "TestFunctionArg" );
		engine.RegisterFunction<decltype(&AngelScriptTest::TestFunctionRet), &AngelScriptTest::TestFunctionRet>( "TestFunctionRet" );
		engine.RegisterFunction<decltype(&AngelScriptTest::TestGetInt), &AngelScriptTest::TestGetInt>( "TestGetInt" );
	}
}

TEST_CASE( "[AngelScript] Register Free Function" )
{
	dd::AngelScriptEngine engine;
	engine.RegisterFunction<decltype(&Test::AngelScript::FreeFunction), &Test::AngelScript::FreeFunction>( "FreeFunction" );
	engine.RegisterFunction<decltype(&Test::AngelScript::FreeFunctionWithArg), &Test::AngelScript::FreeFunctionWithArg>( "FreeFunctionWithArg" );
}

TEST_CASE( "[AngelScript] Register Global Var" )
{
	dd::AngelScriptEngine engine;
	engine.RegisterGlobalVariable<decltype(Test::AngelScript::s_counter), Test::AngelScript::s_counter>( "Counter" );
}

TEST_CASE( "[AngelScript] Evaluate String" )
{
	dd::AngelScriptEngine engine;
	dd::String256 output;
	bool success = engine.Evaluate( dd::String256( "int i = 5; bool b = true;" ), output );

	REQUIRE( success == true );
}

TEST_CASE( "[AngelScript] Call Function" )
{
	// our test script uses other types registered elsewhere
	dd::AngelScriptEngine& engine = dd::Services::Get<dd::AngelScriptEngine>();

	dd::String256 output;
	bool success = engine.LoadFile( "test", output );
	REQUIRE( success == true );

	dd::AngelScriptFunction* fn = engine.GetFunction( "test", "void test()" );
	REQUIRE( fn != nullptr );

	success = (*fn)();
	REQUIRE( success == true );

	delete fn;
}

TEST_CASE( "[AngelScript] Return Values" )
{
	dd::AngelScriptEngine engine;
	dd::String256 output;
	bool success = engine.LoadFile( "test_returns", output );
	REQUIRE( success == true );

	int iRet = 0;
	dd::AngelScriptFunction* iFn = engine.GetFunction( "test_returns", "int test_returns_int()" );
	REQUIRE( iFn != nullptr );

	success = (*iFn)();
	REQUIRE( success == true );

	iFn->Returned( iRet );
	REQUIRE( iRet == 5 );

	delete iFn;

	float fRet = 0;
	dd::AngelScriptFunction* fFn = engine.GetFunction( "test_returns", "float test_returns_float()" );
	REQUIRE( fFn != nullptr );

	success = (*fFn)();
	REQUIRE( success == true );

	fFn->Returned( fRet );
	REQUIRE( fRet == 5.0f );

	delete fFn;

	double dRet = 0;
	dd::AngelScriptFunction* dFn = engine.GetFunction( "test_returns", "double test_returns_double()" );
	REQUIRE( dFn != nullptr );

	success = (*dFn)();
	REQUIRE( success == true );

	dFn->Returned( dRet );
	REQUIRE( dRet == 5 );

	delete dFn;
}

TEST_CASE( "[AngelScript] Args" )
{
	dd::AngelScriptEngine engine;
	dd::String256 output;
	bool success = engine.LoadFile( "test_args", output );
	REQUIRE( success == true );

	int iRet = 0;
	int iA = 1;
	int iB = 2;

	dd::AngelScriptFunction* iFn = engine.GetFunction( "test_args", "int test_args( int x, int y )" );
	REQUIRE( iFn != nullptr );

	success = (*iFn)(iA, iB);
	REQUIRE( success == true );

	iFn->Returned( iRet );
	REQUIRE( iRet == 3 );

	delete iFn;

	float fRet = 0;
	float fA = 2.0f;
	float fB = 3.0f;

	dd::AngelScriptFunction* fFn = engine.GetFunction( "test_args", "float test_args( float x, float y )" );
	REQUIRE( fFn != nullptr );

	success = (*fFn)(fA, fB);
	REQUIRE( success == true );

	fFn->Returned( fRet );
	REQUIRE( fRet == 5.0f );

	delete fFn;
}