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
	}

	int TestFunctionRet()
	{
		Boolean = true;
		return 1;
	}

	void TestFunctionArg( int a, int b )
	{
		Integer = a + b;
	}

	int TestGetInt() const
	{
		return Integer;
	}

	BEGIN_TYPE( WrenTest )
	END_TYPE
};

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

	engine.RegisterType<WrenTest, int, bool>( false );
}

TEST_CASE( "[Wren] Register Member" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, int, bool>( false );
	engine.RegisterMember<WrenTest, int, &WrenTest::Integer>( "Integer" );
	engine.RegisterMember<WrenTest, bool, &WrenTest::Boolean>( "Boolean" );
}

TEST_CASE( "[Wren] Register Free Function" )
{
	dd::WrenEngine engine;

	engine.RegisterFunction<decltype(&FreeFunction), &FreeFunction>( "FreeFunction", FUNCTION( FreeFunction ) );
	engine.RegisterFunction<decltype(&FreeFunctionWithArg), &FreeFunctionWithArg>( "FreeFunctionWithArg", FUNCTION( FreeFunctionWithArg ) );
}

TEST_CASE( "[Wren] Register Method" )
{
	REGISTER_TYPE( WrenTest );

	dd::WrenEngine engine;

	engine.RegisterType<WrenTest, int, bool>( false );
	engine.RegisterFunction<decltype(&WrenTest::TestFunction), &WrenTest::TestFunction>( "TestFunction", FUNCTION( WrenTest::TestFunction ) );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionArg), &WrenTest::TestFunctionArg>( "TestFunctionArg", FUNCTION( WrenTest::TestFunctionArg ) );
	engine.RegisterFunction<decltype(&WrenTest::TestFunctionRet), &WrenTest::TestFunctionRet>( "TestFunctionRet", FUNCTION( WrenTest::TestFunctionRet ) );
	engine.RegisterFunction<decltype(&WrenTest::TestGetInt), &WrenTest::TestGetInt>( "TestGetInt", FUNCTION( WrenTest::TestGetInt ) );
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

	dd::WrenMethod method = engine.GetMethod( dd::String16( "test" ), dd::String16( "test_var" ), dd::String16( "simple" ), 0 );
	method();
}