//
// PlaneTests.cpp - Tests for Plane.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "Tests.h"

#include "ddm/Plane.h"

using ddm::Plane;

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

TEST_CASE( "[Plane] Intersects Line" )
{
	glm::vec3 hit;

	SECTION( "+X" )
	{
		Plane p( 1, 0, 0, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( -1, 0, 0 ), glm::vec3( 1, 0, 0 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 1, 0, 0 ), glm::vec3( -1, 0, 0 ), hit ) );
	}

	SECTION( "-X" )
	{
		Plane p( -1, 0, 0, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( -1, 0, 0 ), glm::vec3( 1, 0, 0 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 1, 0, 0 ), glm::vec3( -1, 0, 0 ), hit ) );
	}

	SECTION( "+Y" )
	{
		Plane p( 0, 1, 0, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( 0, -1, 0 ), glm::vec3( 0, 1, 0 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 0, 1, 0 ), glm::vec3( 0, -1, 0 ), hit ) );
	}

	SECTION( "-Y" )
	{
		Plane p( 0, -1, 0, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( 0, -1, 0 ), glm::vec3( 0, 1, 0 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 0, 1, 0 ), glm::vec3( 0, -1, 0 ), hit ) );
	}

	SECTION( "+Z" )
	{
		Plane p( 0, 0, 1, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( 0, 0, -1 ), glm::vec3( 0, 0, 1 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 0, 0, 1 ), glm::vec3( 0, 0, -1 ), hit ) );
	}

	SECTION( "-Z" )
	{
		Plane p( 0, 0, -1, 0 );

		REQUIRE( p.IntersectsLine( glm::vec3( 0, 0, -1 ), glm::vec3( 0, 0, 1 ), hit ) );
		REQUIRE( p.IntersectsLine( glm::vec3( 0, 0, 1 ), glm::vec3( 0, 0, -1 ), hit ) );
	}
}