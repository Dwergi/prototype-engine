//
// Tests.cpp - Test runner for Catch tests.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Tests.h"
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

int tests::RunTests( int argc, char* argv[] )
{
	Catch::Session session;
	Catch::ConfigData& config = session.configData();
	config.showDurations = Catch::ShowDurations::Always;
	return session.run( argc, argv );
}