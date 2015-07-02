#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "Timer.h"
#include "ScopedTimer.h"

float to_seconds( std::time_t t )
{
	return t / (float) CLOCKS_PER_SEC;
}

void wait( float f )
{
	REQUIRE( f > 0 );

	std::time_t start_check = std::clock();

	float t = 0;
	while( t < f ) 
	{ 
		t = to_seconds( std::clock() - start_check );
	}
}

TEST_CASE( "Timer", "[timer]" )
{
	dd::Timer timer;
	float start = timer.time();
	REQUIRE( start == 0 );

	SECTION( "Counts up" )
	{
		timer.start();
		
		wait( 0.01f );

		float t = timer.time();

		REQUIRE( start < t );
	}

	SECTION( "Stops" )
	{
		timer.start();

		float stopped = timer.stop();

		wait( 0.01f );

		REQUIRE( stopped == timer.time() );

		SECTION( "Resumes" )
		{
			float resumed = timer.time();
			timer.start();

			wait( 0.01f );

			REQUIRE( timer.time() > resumed );
		}
	}
}