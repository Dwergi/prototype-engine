//
// CommandLineTests.h - Tests for the command-line parser.
// Copyright (C) Sebastian Nordgren 
// September 29th 2015
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "CommandLine.h"

TEST_CASE( "[CommandLine] Single Arguments" )
{
	char* args[] = { "-crap", "-hello", "test", "-lol" };

	dd::CommandLine cmdline( args, 4 );

	// first is the program name, should be ignored
	REQUIRE( !cmdline.Exists( "crap" ) );

	REQUIRE( cmdline.Exists( "hello" ) );

	// doesn't start with a dash, should be ignored
	REQUIRE( !cmdline.Exists( "test" ) );

	REQUIRE( cmdline.Exists( "lol" ) );
	REQUIRE( cmdline.GetValue( "lol" ) == "" );
}

TEST_CASE( "[CommandLine] Pairs" )
{
	char* args[] = { "program_name", "-noassert=true", "-test=false" };

	dd::CommandLine cmdline( args, 3 );

	REQUIRE( cmdline.Exists( "noassert" ) );
	REQUIRE( !cmdline.Exists( "lol" ) );

	REQUIRE( cmdline.GetValue( "noassert" ) == "true" ); 

	REQUIRE( cmdline.GetValue( "test" ) == "false" );
}