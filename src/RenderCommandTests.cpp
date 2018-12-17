//
// RenderCommandTests.cpp
// Copyright (C) Sebastian Nordgren 
// December 17th 2016
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "RenderCommand.h"

constexpr uint MASK = 0x00FFFFFF;

TEST_CASE( "[RenderCommand] DistanceToDepth 0" )
{
	uint min_depth = ddr::DistanceToDepth( 0, true );
	REQUIRE( min_depth == 0 );

	uint max_depth = ddr::DistanceToDepth( 0, false );
	REQUIRE( max_depth == (~0 & MASK));
}

TEST_CASE( "[RenderCommand] DistanceToDepth Opaque" )
{
	uint a = ddr::DistanceToDepth( 0, false );
	uint b = ddr::DistanceToDepth( 500, false );
	uint c = ddr::DistanceToDepth( 1000, false );
	uint d = ddr::DistanceToDepth( 2000, false );

	REQUIRE( a > b );
	REQUIRE( b > c );
	REQUIRE( c > d );
}

TEST_CASE( "[RenderCommand] DistanceToDepth Opaque Fractional" )
{
	uint a = ddr::DistanceToDepth( 0, false );
	uint b = ddr::DistanceToDepth( 0.1, false );
	uint c = ddr::DistanceToDepth( 0.5, false );
	uint d = ddr::DistanceToDepth( 0.9, false );

	REQUIRE( a > b );
	REQUIRE( b > c );
	REQUIRE( c > d );
}


TEST_CASE( "[RenderCommand] DistanceToDepth Transparent" )
{
	uint a = ddr::DistanceToDepth( 0, true );
	uint b = ddr::DistanceToDepth( 500, true );
	uint c = ddr::DistanceToDepth( 1000, true );
	uint d = ddr::DistanceToDepth( 2000, true );

	REQUIRE( a < b );
	REQUIRE( b < c );
	REQUIRE( c < d );
}

TEST_CASE( "[RenderCommand] DistanceToDepth Transparent Fractional" )
{
	uint a = ddr::DistanceToDepth( 0, true );
	uint b = ddr::DistanceToDepth( 0.1, true );
	uint c = ddr::DistanceToDepth( 0.5, true );
	uint d = ddr::DistanceToDepth( 0.9, true );

	REQUIRE( a < b );
	REQUIRE( b < c );
	REQUIRE( c < d );
}