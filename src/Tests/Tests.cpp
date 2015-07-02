#include "Tests.h"
#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int tests::RunTests( int argc, char* const argv[] ) 
{
	Catch::Session session;
	int returnCode = session.applyCommandLine( argc, argv );
	if( returnCode != 0 )
		return returnCode;

	return session.run( argc, argv );
}