//
// PlaneTests.cpp - Tests for Plane.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "Plane.h"

using dd::Plane;

TEST_CASE( "[Plane]" )
{
	Plane p;

	SECTION( "Point + Normal" )
	{
		p = Plane( glm::vec3( 0, 0, 0 ), glm::vec3( 1, 0, 0 ) );
	}

	SECTION( "3 Points" )
	{
		p = Plane( glm::vec3( 0, 1, 1 ), glm::vec3( 0, 1, 0 ), glm::vec3( 0, 3, 0 ) );
	}

	SECTION( "Parameters" )
	{
		p = Plane( glm::vec4( 1, 0, 0, 0 ) );
	}

	REQUIRE( p.Normal() == glm::vec3( 1, 0, 0 ) );
	REQUIRE( p.DistanceTo( glm::vec3( 1, 0, 0 ) ) == 1.f );
	REQUIRE( p.DistanceTo( glm::vec3( -1, 0, 0 ) ) == -1.f );
	REQUIRE( p.DistanceTo( glm::vec3( 0, 1, 0 ) ) == 0.0f );
}