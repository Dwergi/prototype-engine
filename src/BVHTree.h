//
// BVHTree.h - Bounding volume hierarchy for AABBs. 
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#pragma once

#include "AABB.h"
#include "Ray.h"

namespace dd
{
	struct Intersection
	{
		int Handle;
		float Distance;
	};

	struct BVHTree
	{
		BVHTree();
		~BVHTree();

		size_t Add( const AABB& bounds );

		void Remove( int handle );

		bool IntersectsRay( const Ray& ray, std::vector<Intersection>& outResults ) const;

		AABB GetEntryBounds( int handle ) const { DD_ASSERT( !IsFreeEntry( handle ) ); return m_entries[ handle ].Bounds; }

		size_t GetEntryCount() const { return m_entries.size() - m_freeEntries.size(); }

		AABB GetBounds() const { return m_buckets[ 0 ].Bounds; }

	private:

		static const size_t MAX_ENTRIES = 8;
		static const size_t INVALID = -1;

		struct BVHEntry
		{
			size_t Handle;
			AABB Bounds;
		};

		struct BVHBucket
		{
			// The region to use for comparisons.
			AABB Region;

			// The actual bounds of the entries in the region.
			AABB Bounds;

			size_t Left { INVALID };
			size_t Right { INVALID };
			dd::Array<size_t, MAX_ENTRIES> Entries;

			bool IsLeaf() const { return Left == INVALID && Right == INVALID; }
		};

		std::vector<BVHEntry> m_entries;
		std::vector<BVHBucket> m_buckets;
		std::vector<size_t> m_freeEntries;
		std::vector<size_t> m_freeBuckets;

		void RebuildTree();
		void SplitBucket( size_t parent_index );
		void InsertEntry( size_t entry_index );
		AABB CalculateBucketBounds( const BVHBucket& bucket ) const;

		bool IsFreeEntry( size_t entry_index ) const;
	};
}