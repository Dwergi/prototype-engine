//
// Octree2Tests.cpp - Tests for Octree2
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "Octree2.h"

#include "Random.h"

#include "catch/catch.hpp"

using namespace dd;

Octree2 octree;

TEST_CASE( "[Octree2] Add" )
{
	RandomInt rng( -500, 500, 2020202020 );

	Vector<OctreeEntry> entries;

	for( int i = 0; i < 256; ++i )
	{
		AABB new_entry;
		new_entry.Min = glm::vec3( rng.Next(), rng.Next(), rng.Next() );
		new_entry.Max = glm::vec3( rng.Next(), rng.Next(), rng.Next() );
		entries.Add( octree.Add( new_entry ) );

		REQUIRE( octree.GetBounds().Contains( new_entry ) );
		REQUIRE( octree.GetEntryCount() == i + 1 );
	}

	uint nodes = octree.GetNodeCount();
}