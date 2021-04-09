//
// TimerTests.h - Tests for Timer.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "Tests.h"

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
	float start = timer.TimeInSeconds();
	REQUIRE( start == 0 );

	SECTION( "Counts up" )
	{
		timer.Start();
		
		wait( 0.01f );

		float t = timer.TimeInSeconds();

		REQUIRE( start < t );
	}

	SECTION( "Stops" )
	{
		timer.Start();

		float stopped = timer.Stop();

		wait( 0.01f );

		REQUIRE( stopped == timer.TimeInSeconds() );

		SECTION( "Resumes" )
		{
			float resumed = timer.TimeInSeconds();
			timer.Start();

			wait( 0.01f );

			REQUIRE( timer.TimeInSeconds() > resumed );
		}
	}
}