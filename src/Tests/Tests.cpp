//
// Tests.cpp - Test runner for Catch tests.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Tests.h"
#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int tests::RunTests( int argc, char const* argv[] )
{
	Catch::Session session;
	return session.run( argc, argv );
}