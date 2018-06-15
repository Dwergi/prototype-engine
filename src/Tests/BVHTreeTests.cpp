//
// BVHTreeTests.cpp - Tests for BVHTree
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PrecompiledHeader.h"
#include "BVHTree.h"

#include "Random.h"

#include "catch/catch.hpp"

TEST_CASE( "[BVHTree] Add" )
{
	dd::BVHTree tree;

	dd::AABB bounds( glm::vec3( -500.0f ), glm::vec3( 500.0f ) );
	dd::RandomInt rng( -500, 500, 1 );

	for( int i = 0; i < 256; ++i )
	{
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	REQUIRE( tree.GetEntryCount() == 256 );
	REQUIRE( bounds.Contains( tree.GetBounds() ) );
}

TEST_CASE( "[BVHTree] Remove" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -500, 500, 1 );

	for( int i = 0; i < 256; ++i )
	{
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	REQUIRE( tree.GetEntryCount() == 256 );

	for( int i = 0; i < 256; ++i )
	{
		tree.Remove( i );
	}

	REQUIRE( tree.GetEntryCount() == 0 );
}