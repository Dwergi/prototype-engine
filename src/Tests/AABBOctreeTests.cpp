//
// AABBOctreeTests.cpp - Tests for AABBOctree
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PCH.h"
#include "Tests.h"

#include "AABBOctree.h"
#include "Random.h"

using namespace dd;

AABBOctree octree;

TEST_CASE( "[AABBOctree] Add" )
{
	std::swap( octree, AABBOctree() );

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
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
		ddm::AABB new_entry;
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

	Vector<ddm::AABB> bounds;

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		bounds.Add( std::move( new_entry ) );
	}

	Vector<OctreeEntry> results;
	Vector<ddm::AABB> comparison;

	for( int i = 0; i < 128; ++i )
	{
		comparison.Clear();
		results.Clear();

		glm::vec3 pt( rng.Next(), rng.Next(), rng.Next() );

		octree.GetAllContaining( pt, results );

		// brute force calculate
		for( const ddm::AABB& aabb : bounds )
		{
			if( aabb.Contains( pt ) )
				comparison.Add( aabb );
		}

		for( int i = 0; i < results.Size(); ++i )
		{
			const ddm::AABB& bb = octree.GetEntry( results[i] );

			REQUIRE( bb.Contains( pt ) );
			REQUIRE( bb == comparison[i] );
		}
	}
}

TEST_CASE( "[AABBOctree] GetIntersecting" )
{
	std::swap( octree, AABBOctree() );

	Vector<ddm::AABB> bounds;

	RandomInt rng( -500, 500, 2020202020 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.Add( new_entry );

		bounds.Add( std::move( new_entry ) );
	}

	Vector<OctreeEntry> results;
	Vector<ddm::AABB> comparison;

	for( uint i = 0; i < 128; ++i )
	{
		comparison.Clear();
		results.Clear();

		ddm::AABB test;
		test.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		test.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		octree.GetAllIntersecting( test, results );

		// brute force calculate
		for( const ddm::AABB& aabb : bounds )
		{
			if( aabb.Intersects( test ) )
				comparison.Add( aabb );
		}

		for( int i = 0; i < results.Size(); ++i )
		{
			const ddm::AABB& bb = octree.GetEntry( results[i] );

			REQUIRE( bb.Intersects( test ) );
			REQUIRE( bb == comparison[i] );
		}
	}
}