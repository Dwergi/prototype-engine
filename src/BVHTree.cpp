//
// BVHTree.h - Bounding volume hierarchy for AABBs. 
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PCH.h"
#include "BVHTree.h"

#include "AABB.h"
#include "JobSystem.h"

static dd::Service<dd::JobSystem> s_jobsystem;

namespace dd
{
	BVHTree::BVHTree()
	{
		ClearBuckets();
	}

	BVHTree::~BVHTree()
	{

	}

	BVHHandle BVHTree::Add(const ddm::AABB& bounds)
	{
		BVHEntry& entry = m_entries.emplace_back(BVHEntry());
		entry.Bounds = bounds;
		entry.Handle = (BVHHandle) (m_entries.size() - 1);

		m_buckets[0].Region.Expand(bounds);

		return entry.Handle;
	}

	void BVHTree::ClearBuckets()
	{
		m_buckets.clear();
		m_buckets.emplace_back(BVHBucket());
		m_bucketCount = 1;

		m_built = false;
	}

	void BVHTree::Clear()
	{
		m_entries.clear();

		ClearBuckets();
	}

	BVHIntersection BVHTree::IntersectsRay(const ddm::Ray& ray) const
	{
		DD_ASSERT(m_built);

		dd::Array<size_t, 64> stack;
		stack.Add(0);

		BVHIntersection nearest;
		nearest.Distance = FLT_MAX;
		nearest.Handle = BVH::Invalid;

		int buckets_tested = 0;

		while (stack.Size() > 0)
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];

			float bucket_distance;
			if (bucket.Bounds.IntersectsRay(ray, bucket_distance) &&
				bucket_distance < nearest.Distance &&
				bucket_distance < ray.Length)
			{
				if (bucket.IsLeaf)
				{
					for (size_t e = bucket.Left; e < bucket.Right; ++e)
					{
						float distance;
						const BVHEntry& entry = m_entries[e];
						if (entry.Bounds.IntersectsRay(ray, distance) &&
							distance < nearest.Distance)
						{
							nearest.Handle = entry.Handle;
							nearest.Distance = distance;
						}
					}
				}
				else
				{
					stack.Add(bucket.Left);
					stack.Add(bucket.Right);
				}
			}
		}

		//DD_DIAGNOSTIC("[BVHTree] IntersectsRay - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return nearest;
	}

	BVHIntersection BVHTree::IntersectsRayFn(const ddm::Ray& ray, const HitTestFn& fn) const
	{
		DD_ASSERT(m_built);

		dd::Array<size_t, 64> stack;
		stack.Add(0);

		BVHIntersection nearest;
		nearest.Distance = FLT_MAX;
		nearest.Handle = BVH::Invalid;

		int buckets_tested = 0;

		while (stack.Size() > 0)
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];

			float bucket_distance;
			if (bucket.Bounds.IntersectsRay(ray, bucket_distance) &&
				bucket_distance < nearest.Distance &&
				bucket_distance < ray.Length)
			{
				if (bucket.IsLeaf)
				{
					for (size_t e = bucket.Left; e < bucket.Right; ++e)
					{
						const BVHEntry& entry = m_entries[e];
						float entry_distance;
						if (entry.Bounds.IntersectsRay(ray, entry_distance) &&
							entry_distance < nearest.Distance)
						{
							float actual_distance = fn(entry.Handle);
							if (actual_distance < nearest.Distance)
							{
								nearest.Handle = entry.Handle;
								nearest.Distance = actual_distance;
							}
						}
					}
				}
				else
				{
					stack.Add(bucket.Left);
					stack.Add(bucket.Right);
				}
			}
		}

		//DD_DIAGNOSTIC("[BVHTree] IntersectsRay - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return nearest;
	}

	bool BVHTree::WithinBoundBox(const ddm::AABB& bounds, std::vector<BVHHandle>& out_hits) const
	{
		DD_ASSERT(m_built);

		int buckets_tested = 0;

		dd::Array<size_t, 64> stack;
		stack.Add(0);

		bool hit = false;

		while (stack.Size() > 0)
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];

			if (bucket.Bounds.Intersects(bounds))
			{
				if (bucket.IsLeaf)
				{
					for (size_t e = bucket.Left; e < bucket.Right; ++e)
					{
						const BVHEntry& entry = m_entries[e];
						if (entry.Bounds.Intersects(bounds))
						{
							out_hits.push_back(entry.Handle);
							hit = true;
						}
					}
				}
				else
				{
					stack.Add(bucket.Left);
					stack.Add(bucket.Right);
				}
			}
		}

		//DD_DIAGNOSTIC("[BVHTree] WithinBoundBox - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return hit;
	}

	bool BVHTree::WithinBoundSphere(const ddm::Sphere& sphere, std::vector<BVHHandle>& out_hits) const
	{
		DD_ASSERT(m_built);

		int buckets_tested = 0;

		dd::Array<size_t, 64> stack;
		stack.Add(0);

		bool hit = false;

		while (stack.Size() > 0)
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];
			if (bucket.Bounds.Intersects(sphere))
			{
				if (bucket.IsLeaf)
				{
					for (size_t e = bucket.Left; e < bucket.Right; ++e)
					{
						const BVHEntry& entry = m_entries[e];
						if (entry.Bounds.Intersects(sphere))
						{
							out_hits.push_back(entry.Handle);
							hit = true;
						}
					}
				}
				else
				{
					stack.Add(bucket.Left);
					stack.Add(bucket.Right);
				}
			}
		}

		//DD_DIAGNOSTIC("[BVHTree] WithinBoundSphere - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return hit;
	}

	void BVHTree::CalculateBucketBounds(BVHBucket& bucket)
	{
		bucket.Bounds.Clear();

		if (bucket.IsLeaf)
		{
			for (size_t e = bucket.Left; e < bucket.Right; ++e)
			{
				bucket.Bounds.Expand(m_entries[e].Bounds);
			}
		}
		else
		{
			bucket.Bounds.Expand(m_buckets[bucket.Left].Bounds);
			bucket.Bounds.Expand(m_buckets[bucket.Right].Bounds);
		}
	}

#pragma optimize("", off)
	void BVHTree::SplitBucket(size_t parent_idx)
	{
		BVHBucket& parent = m_buckets[parent_idx];
		DD_ASSERT(parent.IsLeaf);
		DD_ASSERT(parent.Region.IsValid());
		DD_TODO("Might be better to make this stack-based?");

		// recursion condition
		if (parent.Count() <= MAX_ENTRIES)
		{
			return;
		}

		// figure out the split axis
		glm::vec3 half_extents = parent.Region.Extents() / 2.0f;

		int split_axis = 0;
		if (half_extents.z >= half_extents.x && half_extents.z >= half_extents.y)
		{
			split_axis = 2;
		}
		else if (half_extents.y >= half_extents.x && half_extents.y >= half_extents.z)
		{
			split_axis = 1;
		}

		float split_coord = parent.Region.Min[split_axis] + half_extents[split_axis];
		parent.SplitAxis = (Axis) split_axis;

		size_t start = parent.Left;
		size_t mid = start;
		size_t end = parent.Right;

		// create sub-buckets
		size_t left_index = m_bucketCount++;
		BVHBucket& left = m_buckets[left_index];
		left.Region.Min = parent.Region.Min;
		left.Region.Max = parent.Region.Max;
		left.Region.Max[split_axis] = split_coord;

		left.IsLeaf = true;
		left.Left = start;
		left.Right = start;

		size_t right_index = m_bucketCount++;
		BVHBucket right = m_buckets[right_index];
		right.Region.Min = parent.Region.Min;
		right.Region.Min[split_axis] = split_coord;
		right.Region.Max = parent.Region.Max;

		right.IsLeaf = true;
		right.Left = end;
		right.Right = end;

		parent.IsLeaf = false;
		parent.Left = left_index;
		parent.Right = right_index;

		// split the current entries between the two buckets
		for (size_t i = start; i < end; ++i)
		{
			glm::vec3 center = m_entries[i].Bounds.Center();
			if (center[split_axis] < split_coord)
			{
				std::swap(m_entries[mid], m_entries[i]);
				++mid;
			}
		}

		// bad split, just arbitrarily use the middle
		if (mid == start || mid == end)
		{
			mid = start + (end - start) / 2;
		}

		DD_ASSERT(mid != start && mid != end);

		left.Right = mid;
		right.Left = mid;

		if (left.Count() > 512)
		{
			s_jobsystem->ScheduleMethodChild(m_job, this, &BVHTree::SplitBucket, left_index);
		}
		else
		{
			SplitBucket(left_index);
		}

		if (right.Count() > 512)
		{
			s_jobsystem->ScheduleMethodChild(m_job, this, &BVHTree::SplitBucket, right_index);
		}
		else
		{
			SplitBucket(right_index);
		}
	}

	void BVHTree::Build()
	{
		if (m_built)
		{
			ClearBuckets();
		}

		if (m_entries.size() == 0)
		{
			m_built = true;
			return;
		}

		m_buckets.resize(m_entries.size() * 2);

		BVHBucket& root = m_buckets[0];
		root.Bounds = root.Region;
		root.IsLeaf = true;
		root.Left = 0;
		root.Right = m_entries.size();

		DD_ASSERT(AllBucketsEmpty());

		m_job = s_jobsystem->CreateMethod(this, &BVHTree::SplitBucket, (size_t) 0);
		s_jobsystem->Schedule(m_job);
		s_jobsystem->Wait(m_job);

		m_job = nullptr;

		m_buckets.resize(m_bucketCount);

		for (size_t b = m_buckets.size(); b > 0; --b)
		{
			CalculateBucketBounds(m_buckets[b - 1]);
		}

		m_built = true;
	}

	void BVHTree::CountBucketSplits(int& x, int& y, int& z) const
	{
		x = y = z = 0;

		for (const BVHBucket& bucket : m_buckets)
		{
			if (bucket.SplitAxis == Axis::X)
			{
				++x;
			}
			else if (bucket.SplitAxis == Axis::Y)
			{
				++y;
			}
			else
			{
				++z;
			}
		}
	}

	bool BVHTree::AllBucketsEmpty() const
	{
		for (size_t i = 1; i < m_buckets.size(); ++i)
		{
			const BVHBucket& bucket = m_buckets[i];
			if (!bucket.IsLeaf || bucket.Left != bucket.Right)
			{
				return false;
			}
		}

		return true;
	}
}