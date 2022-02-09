//
// BVHTreeTests.cpp - Tests for BVHTree
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PCH.h"
#include "Tests.h"

#include "BVHTree.h"
#include "Random.h"
#include "ddm/Ray.h"

#if 0

TEST_CASE("[BVHTree] Controlled Build")
{
	dd::BVHTree tree;

	for (float f = 1; f < 20; f += 1)
	{
		tree.Add(ddm::AABB(glm::vec3(f), glm::vec3(f + 1)));
		tree.Add(ddm::AABB(glm::vec3(-f), glm::vec3(-f - 1)));
	}

	tree.Build();
}

TEST_CASE( "[BVHTree] Add" )
{
	dd::BVHTree tree;

	ddm::AABB bounds( glm::vec3( -500.0f ), glm::vec3( 500.0f ) );
	dd::RandomInt rng( -500, 500, 1 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	tree.Build();

	REQUIRE( tree.GetEntryCount() == 256 );
	REQUIRE( bounds.Contains( tree.GetBVHBounds() ) );
}

TEST_CASE( "[BVHTree] Intersects" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( 100, 500, 1 );

	ddm::AABB entry_bb( glm::vec3( 0, 0, 0 ), glm::vec3( 10, 10, 10 ) );
	dd::BVHHandle entry_h = tree.Add( entry_bb );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	tree.Build();

	ddm::Ray ray( glm::vec3( 0, -5, 0 ), glm::vec3( 0, 1, 0 ) );

	float distance;
	REQUIRE( entry_bb.IntersectsRay( ray, distance ) );

	dd::BVHIntersection hit = tree.IntersectsRay( ray );
	REQUIRE( hit.IsValid() );
	REQUIRE( hit.Handle == entry_h );
	REQUIRE( hit.Distance == 5.0f );
}

TEST_CASE( "[BVHTree] Misses Root" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -500, 500, 1 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	tree.Build();

	ddm::Ray ray( glm::vec3( 1000, 1000, 1000 ), glm::vec3( 0, 1, 0 ) );

	dd::BVHIntersection hit = tree.IntersectsRay( ray );
	REQUIRE_FALSE( hit.IsValid() );
}

TEST_CASE( "[BVHTree] Huge Tree" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -5000, 5000, 1 );

	for( int i = 0; i < 2 * 1024; ++i )
	{
		ddm::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	tree.Build();

	for( int i = 0; i < 256; ++i )
	{
		ddm::Ray ray( glm::vec3( rng.Next(), rng.Next(), rng.Next() ), glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.IntersectsRay( ray );
	}

	DD_DIAGNOSTIC("Entries: %zu, Buckets: %zu\n", tree.GetEntryCount(), tree.GetBucketCount());

	int x, y, z;
	tree.CountBucketSplits(x, y, z);

	int most = ddm::max(x, ddm::max(y, z));
	int least = ddm::min(x, ddm::min(y, z));

	//REQUIRE((size_t) (most - least) < (tree.GetBucketCount() / 10));
}

TEST_CASE( "[BVHTree] Within Bounds" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -500, 500, 1 );
	dd::RandomInt rng_size( 1, 10, 1 );

	for( int i = 0; i < 256; ++i )
	{
		ddm::AABB new_entry;
		glm::vec3 pos( rng.Next(), rng.Next(), rng.Next() );
		new_entry.Expand( pos );
		new_entry.Expand( pos + glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	tree.Build();

	ddm::AABB test( glm::vec3( -10 ), glm::vec3( 10 ) );
	std::vector<dd::BVHHandle> hits;

	REQUIRE( tree.WithinBoundBox( test, hits ) );
}

#endif