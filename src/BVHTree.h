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
	struct Job;

	enum class BVHHandle : uint32 {};

	namespace BVH
	{
		constexpr BVHHandle Invalid = (BVHHandle) -1;
	}

	struct BVHIntersection
	{
		BVHHandle Handle { BVH::Invalid };
		float Distance { 0 };

		bool IsValid() const { return Handle != BVH::Invalid; }
	};

	using HitTestFn = std::function<float(BVHHandle)>;

	//
	// BVH that cannot be removed from (because it simplifies things, like, a lot).
	// 
	struct BVHTree
	{
		BVHTree();
		~BVHTree();

		BVHHandle Add(const ddm::AABB& bounds);
		void Build();

		void Clear();

		BVHIntersection IntersectsRay(const ddm::Ray& ray) const;
		BVHIntersection IntersectsRayFn(const ddm::Ray& ray, const HitTestFn& fn) const;

		bool WithinBoundBox(const ddm::AABB& bounds, std::vector<BVHHandle>& out_hits) const;
		bool WithinBoundSphere(const ddm::Sphere& sphere, std::vector<BVHHandle>& out_hits) const;

		ddm::AABB GetBVHBounds() { return m_buckets[0].Bounds; }

		size_t GetEntryCount() const { return m_entries.size(); }

		//
		// Reserve a number of entries to avoid allocations.
		// 
		void Reserve(size_t count)
		{
			m_entries.reserve(count);
			m_buckets.reserve(count * 2);
		}

		// diagnostics
		void CountBucketSplits(int& x, int& y, int& z) const;
		size_t GetBucketCount() const { return m_buckets.size(); }

	private:

		static const size_t MAX_ENTRIES = 8;

		struct BVHEntry
		{
			dd::BVHHandle Handle { BVH::Invalid };
			ddm::AABB Bounds;
		};

		enum class Axis : int8
		{
			None = -1,
			X,
			Y,
			Z
		};

		struct BVHBucket
		{
			static const size_t INVALID = -1;

			BVHBucket() = default;
			BVHBucket(const BVHBucket&) = delete;
			BVHBucket(BVHBucket&&) = default;

			// The region to use for comparisons.
			ddm::AABB Region;

			// The actual bounds of the entries in the region.
			ddm::AABB Bounds;

			bool IsLeaf { true };

			// If leaf, the start of the entries in this bucket.
			// If not leaf, left bucket index.
			size_t Left { INVALID };

			// If leaf, the end of the entries in this bucket.
			// If not leaf, right bucket index.
			size_t Right { INVALID };

			// The axis along which this node was split. (Diagnostic)
			Axis SplitAxis { Axis::None };

			size_t Count() const { return Right - Left; }
			bool IsEmpty() const { return IsLeaf && Count() == 0; }
		};

		std::vector<BVHEntry> m_entries;
		std::vector<BVHBucket> m_buckets;

		bool m_built { false };
		std::atomic<int> m_bucketCount { 0 };
		std::atomic<int> m_futureCount { 0 };

		void ClearBuckets();

		void SplitBucket(size_t parent_idx, dd::Job* parent_job);
		void CalculateBucketBounds(BVHBucket& bucket);
		bool AllBucketsEmpty() const;
	};
}