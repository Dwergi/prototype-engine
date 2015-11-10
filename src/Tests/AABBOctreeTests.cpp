//
// AABBOctreeTests.cpp - Tests for AABBOctree
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "AABBOctree.h"

#include "Random.h"

#include "catch/catch.hpp"

using namespace dd;

AABBOctree octree;

TEST_CASE( "[AABBOctree] Add" )
{
	std::swap( octree, AABBOctree() );

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		REQUIRE( octree.GetBounds().Contains( new_entry ) );
		REQUIRE( octree.GetEntryCount() == i + 1 );
	}
	
	for( int i = 0; i < 128; ++i )
	{
		octree.Remove( OctreeEntry( i ) );

		REQUIRE( octree.GetEntryCount() == 256 - (i + 1) );
	}

	for( int i = 0; i < 128; ++i )
	{
		AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		uint count = octree.GetEntryCount();
		REQUIRE( count == 128 + i + 1 );
	}
}

TEST_CASE( "[AABBOctree] GetContaining" )
{
	std::swap( octree, AABBOctree() );

	Vector<AABB> bounds;

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		bounds.Add( std::move( new_entry ) );
	}

	Vector<OctreeEntry> results;
	Vector<AABB> comparison;

	for( uint i = 0; i < 128; ++i )
	{
		comparison.Clear();
		results.Clear();

		glm::vec3 pt( rng.Next(), rng.Next(), rng.Next() );

		octree.GetAllContaining( pt, results );

		// brute force calculate
		for( const AABB& aabb : bounds )
		{
			if( aabb.Contains( pt ) )
				comparison.Add( aabb );
		}

		for( uint i = 0; i < results.Size(); ++i )
		{
			const AABB& bb = octree.GetEntry( results[i] );

			REQUIRE( bb.Contains( pt ) );
			REQUIRE( bb == comparison[i] );
		}
	}
}

TEST_CASE( "[AABBOctree] GetIntersecting" )
{
	std::swap( octree, AABBOctree() );

	Vector<AABB> bounds;

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		bounds.Add( std::move( new_entry ) );
	}

	Vector<OctreeEntry> results;
	Vector<AABB> comparison;

	for( uint i = 0; i < 128; ++i )
	{
		comparison.Clear();
		results.Clear();

		AABB test;
		test.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		test.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.GetAllIntersecting( test, results );

		// brute force calculate
		for( const AABB& aabb : bounds )
		{
			if( aabb.Intersects( test ) )
				comparison.Add( aabb );
		}

		for( uint i = 0; i < results.Size(); ++i )
		{
			const AABB& bb = octree.GetEntry( results[i] );

			REQUIRE( bb.Intersects( test ) );
			REQUIRE( bb == comparison[i] );
		}
	}
}