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

TEST_CASE( "[AngelScript] Call Method" )
{
	dd::AngelScriptEngine engine;

	dd::String256 output;
	bool success = engine.LoadFile( "test", output );
	REQUIRE( success == true );

	success = engine.RunFunction( "test", dd::String128( "void test()" ), output );
	REQUIRE( success == true );
}