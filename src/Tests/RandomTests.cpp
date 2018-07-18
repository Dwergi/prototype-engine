//
// RandomTests.h - Tests for the RNG.
// Copyright (C) Sebastian Nordgren 
// August 11th 2015
//

#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

#include "Random.h"

TEST_CASE( "[Random] Bounds" )
{
	dd::Random32 rng( 0, 100 );

	for( int i = 0; i < 100; ++i )
	{
		int number = rng.Next();

		REQUIRE( number >= 0 );
		REQUIRE( number <= 100 );
	}

	dd::Random32 rng2( 100, 250 );

	for( int i = 100; i < 250; ++i )
	{
		int number = rng2.Next();

		REQUIRE( number >= 100 );
		REQUIRE( number <= 250 );
	}
}

TEST_CASE( "[Random] Negative" )
{
	dd::RandomInt rng( -200, -100 );

	for( int i = 0; i < 100; ++i )
	{
		int number = rng.Next();

		REQUIRE( number >= -200 );
		REQUIRE( number <= -100 );
	}
}

TEST_CASE( "[Random] 64 Bit" )
{
	uint64 min = UINT_MAX;
	dd::Random64 rng( min, min * 2 );

	for( int i = 0; i < 100; ++i )
	{
		uint64 number = rng.Next();

		REQUIRE( number >= min );
		REQUIRE( number <= min * 2 );
	}
}

TEST_CASE( "[Random] Integer" )
{
	int min = -100;
	int max = 100;

	dd::RandomInt rng( min, max );

	for( int i = 0; i < 100; ++i )
	{
		int number = rng.Next();

		REQUIRE( number >= min );
		REQUIRE( number <= max );
	}
}

TEST_CASE( "[Random] Seed" )
{
	dd::Random32 rng( 0, 100, 5 );
	
	int first = rng.Next();
	int second = rng.Next();
	int third = rng.Next();
	int fourth = rng.Next();

	dd::Random32 rng2( 0, 100, 5 );

	REQUIRE( rng2.Next() == first );
	REQUIRE( rng2.Next() == second );
	REQUIRE( rng2.Next() == third );
	REQUIRE( rng2.Next() == fourth );
}

TEST_CASE( "[Random] Float" )
{
	float min = -1000.f;
	float max = 1000.f;

	dd::RandomFloat rng( min, max );

	for( int i = 0; i < 100; ++i )
	{
		float number = rng.Next();

		REQUIRE( number >= min );
		REQUIRE( number <= max );
	}
}