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
	struct BVHIntersection
	{
		size_t Handle;
		float Distance;

		bool IsValid() const { return Handle != -1; }
	};

	typedef std::function<float(size_t)> HitTestFn;

	struct BVHTree
	{
		BVHTree();
		~BVHTree();

		size_t Add( const AABB& bounds );
		void Remove( size_t handle );

		BVHIntersection IntersectsRay( const Ray& ray ) const;
		BVHIntersection IntersectsRayFn( const Ray& ray, const HitTestFn& fn ) const;

		bool WithinBounds( const AABB& bounds, std::vector<size_t>& outHits ) const;
		bool WithinBounds( const Sphere& sphere, std::vector<size_t>& outHits ) const;

		AABB GetEntryBounds( int handle ) const { DD_ASSERT( !IsFreeEntry( handle ) ); return m_entries[ handle ].Bounds; }
		AABB GetBounds() const { return m_buckets[ 0 ].Bounds; }

		size_t GetEntryCount() const { return m_entries.size() - m_freeEntries.size(); }
		size_t GetBucketCount() const { return m_buckets.size() - m_freeBuckets.size(); }

		//
		// Reserve a number of entries to avoid allocations.
		// 
		void Reserve( size_t count )
		{
			m_entries.reserve( count );
			m_buckets.reserve( count * 2 );
		}

		//
		// Start a batch operation, which will prevent the tree from being rebuilt.
		//
		void StartBatch() { m_batch = true; }

		//
		// End a batch and force the tree to rebuild.
		//
		void EndBatch() { m_batch = false; RebuildTree(); }

		// diagnostics
		void CountBucketSplits( int& x, int& y, int& z ) const;
		void EnsureAllBucketsEmpty() const;
		int GetRebuildCount() const { return m_rebuildCount; }

	private:

		static const size_t MAX_ENTRIES = 8;
		static const size_t INVALID = -1;

		struct BVHEntry
		{
			AABB Bounds;
		};

		enum class Axis
		{
			X,
			Y,
			Z
		};

		struct BVHBucket
		{
			// The region to use for comparisons.
			AABB Region;

			// The actual bounds of the entries in the region.
			AABB Bounds;

			size_t Parent { INVALID };
			size_t Left { INVALID };
			size_t Right { INVALID };
			dd::Array<size_t, MAX_ENTRIES> Entries;

			// The axis along which this node was split. (Diagnostic)
			Axis SplitAxis { Axis::X };

			bool IsLeaf() const { return Left == INVALID && Right == INVALID; }
			bool IsEmpty() const { return IsLeaf() && Entries.Size() == 0; }
		};

		std::vector<BVHEntry> m_entries;
		std::vector<BVHBucket> m_buckets;
		std::vector<size_t> m_freeEntries;
		std::vector<size_t> m_freeBuckets;

		bool m_batch { false };
		int m_rebuildCount { 0 };

		void RebuildTree();
		void SplitBucket( size_t parent_index );
		void InsertEntry( size_t entry_index );
		AABB CalculateBucketBounds( const BVHBucket& bucket ) const;
		void MergeEmptyBuckets( size_t parent_index );

		bool IsFreeEntry( size_t entry_index ) const;
		bool IsFreeBucket( size_t bucket_index ) const;
	};
}