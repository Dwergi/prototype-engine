//
// BVHTreeTests.cpp - Tests for BVHTree
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PrecompiledHeader.h"
#include "BVHTree.h"

#include "Random.h"
#include "Ray.h"

#include "catch2/catch.hpp"

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

TEST_CASE( "[BVHTree] Remove and Add" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -500, 500, 1 );

	dd::Array<dd::AABB, 256> entries;

	for( int i = 0; i < 256; ++i )
	{
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		entries.Add( new_entry );
		tree.Add( new_entry );
	}

	size_t bucket_count = tree.GetBucketCount();
	REQUIRE( tree.GetEntryCount() == 256 );

	for( int i = 0; i < 256; ++i )
	{
		tree.Remove( i );
	}

	tree.EnsureAllBucketsEmpty();

	REQUIRE( tree.GetBucketCount() == 1 );
	REQUIRE( tree.GetEntryCount() == 0 );

	for( const dd::AABB& entry : entries )
	{
		tree.Add( entry );
	}

	// tree building should be deterministic
	REQUIRE( tree.GetBucketCount() == bucket_count );
}

TEST_CASE( "[BVHTree] Intersects" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( 100, 500, 1 );

	dd::AABB entry_bb( glm::vec3( 0, 0, 0 ), glm::vec3( 10, 10, 10 ) );
	size_t entry_h = tree.Add( entry_bb );

	for( int i = 0; i < 256; ++i )
	{
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	dd::Ray ray( glm::vec3( 0, -5, 0 ), glm::vec3( 0, 1, 0 ) );

	float distance;
	REQUIRE( entry_bb.IntersectsRay( ray, distance ) );

	dd::Intersection hit = tree.IntersectsRay( ray );
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
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	dd::Ray ray( glm::vec3( 1000, 1000, 1000 ), glm::vec3( 0, 1, 0 ) );

	dd::Intersection hit = tree.IntersectsRay( ray );
	REQUIRE_FALSE( hit.IsValid() );
}

TEST_CASE( "[BVHTree] Huge Tree" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -5000, 5000, 1 );

	for( int i = 0; i < 2 * 1024; ++i )
	{
		dd::AABB new_entry;
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
		new_entry.Expand( glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	DD_DIAGNOSTIC( "Entries: %zu, Buckets: %zu, Tree Rebuilds: %d\n", tree.GetEntryCount(), tree.GetBucketCount(), tree.GetRebuildCount() );

	int x, y, z;
	tree.CountBucketSplits( x, y, z );

	int most = dd::max( x, dd::max( y, z ) );
	int least = dd::min( x, dd::min( y, z ) );

	REQUIRE( (size_t) (most - least) < (tree.GetBucketCount() / 10) );

	for( int i = 0; i < 256; ++i )
	{
		dd::Ray ray( glm::vec3( rng.Next(), rng.Next(), rng.Next() ), glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.IntersectsRay( ray );
	}
}

TEST_CASE( "[BVHTree] Within Bounds" )
{
	dd::BVHTree tree;

	dd::RandomInt rng( -500, 500, 1 );
	dd::RandomInt rng_size( 1, 10, 1 );

	for( int i = 0; i < 256; ++i )
	{
		dd::AABB new_entry;
		glm::vec3 pos( rng.Next(), rng.Next(), rng.Next() );
		new_entry.Expand( pos );
		new_entry.Expand( pos + glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );

		tree.Add( new_entry );
	}

	dd::AABB test( glm::vec3( -10 ), glm::vec3( 10 ) );
	std::vector<size_t> hits;

	REQUIRE( tree.WithinBounds( test, hits ) );
}
