#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "WrenEngine.h"

struct WrenTest
{
	int Integer;
	bool Boolean;

	WrenTest()
	{
	}

	WrenTest( int integer, bool boolean )
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

	BEGIN_TYPE( WrenTest )
	END_TYPE
};

bool WrenTest::FunctionCalled = false;
bool WrenTest::FunctionRetCalled = false;
bool WrenTest::FunctionArgCalled = false;

int FreeFunction()
{
	return 5;
}

int FreeFunctionWithArg( int a )
{
	return a * 5;
}

extern uint s_counter;
uint s_counter = 0;

TEST_CASE( "[Wren] Register Class" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, false, int, bool>();
}

TEST_CASE( "[Wren] Register Member" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, false, int, bool>();
	engine.RegisterMember<WrenTest, int, &WrenTest::Integer>( "Integer" );
	engine.RegisterMember<WrenTest, bool, &WrenTest::Boolean>( "Boolean" );
}

TEST_CASE( "[Wren] Register Free Function" )
{
	dd::WrenEngine engine;

	engine.RegisterFunction<decltype(&FreeFunction), &FreeFunction>( "FreeFunction" );
	engine.RegisterFunction<decltype(&FreeFunctionWithArg), &FreeFunctionWithArg>( "FreeFunctionWithArg" );
}

TEST_CASE( "[Wren] Register Method" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, false, int, bool>();
	engine.RegisterFunction<decltype(&WrenTest::TestFunction), &WrenTest::TestFunction>( "TestFunction" );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionArg), &WrenTest::TestFunctionArg>( "TestFunctionArg" );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionRet), &WrenTest::TestFunctionRet>( "TestFunctionRet" );
	engine.RegisterFunction<decltype(&WrenTest::TestGetInt), &WrenTest::TestGetInt>( "TestGetInt" );
}

TEST_CASE( "[Wren] Register Global Var" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterGlobalVariable<decltype( s_counter ), s_counter>( "Counter" );
}

TEST_CASE( "[Wren] Call Wren Method" )
{
	dd::WrenEngine engine;

	dd::String256 output;
	bool res = engine.RunModule( "test", output );

	REQUIRE( res == true );

	output.Clear();

	dd::WrenMethod simple = engine.GetMethod( "test", "test_var", "simple", 0 );
	simple();

	REQUIRE( output.Length() > 0 );

	output.Clear();

	dd::WrenMethod with_arg = engine.GetMethod( "test", "test_var", "with_arg", 1 );
	with_arg( 5 );

	REQUIRE( output.Find( "5" ) != -1 );

	output.Clear();

	dd::WrenMethod multiple_args = engine.GetMethod( "test", "test_var", "multiple_args", 2 );
	multiple_args( 5, "b" );

	REQUIRE( output.Find( "5" ) != -1 );
	REQUIRE( output.Find( "b" ) != -1 );

	output.Clear();

	dd::WrenMethod with_return = engine.GetMethod( "test", "test_var", "with_return", 0 );
	with_return();

	REQUIRE( output.Find( "5" ) != -1 );
}

TEST_CASE( "[Wren] Call From Wren" )
{
	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, false, int, bool>();
	engine.RegisterMember<WrenTest, int, &WrenTest::Integer>( "Integer" );
	engine.RegisterMember<WrenTest, bool, &WrenTest::Boolean>( "Boolean" );
	engine.RegisterFunction<decltype(&WrenTest::TestFunction), &WrenTest::TestFunction>( "TestFunction" );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionArg), &WrenTest::TestFunctionArg>( "TestFunctionArg" );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionRet), &WrenTest::TestFunctionRet>( "TestFunctionRet" );
	engine.RegisterFunction<decltype(&WrenTest::TestGetInt), &WrenTest::TestGetInt>( "TestGetInt" );

	dd::String256 output;
	bool res = engine.RunModule( "test_foreign", output );

	REQUIRE( res == true );

	WrenTest::FunctionCalled = false;
	WrenTest::FunctionRetCalled = false;
	WrenTest::FunctionArgCalled = false;

	dd::WrenMethod start_test = engine.GetMethod( "test_foreign", "test_var", "call_cpp", 0 );
	start_test();

	REQUIRE( WrenTest::FunctionCalled );
	REQUIRE( WrenTest::FunctionRetCalled );
	REQUIRE( WrenTest::FunctionArgCalled );
}