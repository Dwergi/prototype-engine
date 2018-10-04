//
// BVHTree.h - Bounding volume hierarchy for AABBs. 
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PCH.h"
#include "BVHTree.h"

#include "AABB.h"

#include <algorithm>

namespace dd
{
	BVHTree::BVHTree()
	{
		m_buckets.emplace_back( BVHBucket() );
	}

	BVHTree::~BVHTree()
	{

	}

	size_t BVHTree::Add( const AABB& bounds )
	{
		if( m_freeEntries.empty() )
		{
			m_freeEntries.push_back( (int) m_entries.size() );
			m_entries.emplace_back( BVHEntry() );
		}

		size_t index = dd::pop_front( m_freeEntries );

		BVHEntry& entry = m_entries[ index ];
		entry.Bounds = bounds;

		if( m_batch )
		{
			return index;
		}

		DD_ASSERT( m_buckets[ 0 ].Region == m_buckets[ 0 ].Bounds );

		if( !m_buckets[ 0 ].Region.Contains( entry.Bounds ) )
		{
			// root is too small, need to rebuild
			RebuildTree();
		}
		else
		{
			InsertEntry( index );
		}

		return index;
	}

	void BVHTree::MergeEmptyBuckets( size_t bucket_index )
	{
		if( bucket_index != INVALID )
		{
			BVHBucket& bucket = m_buckets[ bucket_index ];

			if( bucket.IsLeaf() )
			{
				if( bucket.Entries.Size() == 0 )
				{
					MergeEmptyBuckets( bucket.Parent );
				}
			}
			else if( m_buckets[ bucket.Left ].IsEmpty() && m_buckets[ bucket.Right ].IsEmpty() )
			{
				// both children empty, merge
				m_freeBuckets.push_back( bucket.Left );
				m_freeBuckets.push_back( bucket.Right );

				bucket.Left = INVALID;
				bucket.Right = INVALID;

				MergeEmptyBuckets( bucket.Parent );
			}
		}
	}

	void BVHTree::Remove( size_t handle )
	{
		if( IsFreeEntry( handle ) )
		{
			return;
		}

		BVHEntry& entry = m_entries[ handle ];
		glm::vec3 center = entry.Bounds.Center();

		// find the bucket
		size_t current_index = 0;
		while( true )
		{
			BVHBucket& current = m_buckets[ current_index ];
			if( current.IsLeaf() )
			{
				break;
			}
			
			if( m_buckets[ current.Left ].Region.Contains( center ) )
			{
				current_index = current.Left;
			}
			else
			{
				current_index = current.Right;
			}
		}

		BVHBucket& bucket = m_buckets[ current_index ];

		uint remove_at = -1;
		for( uint i = 0; i < bucket.Entries.Size(); ++i )
		{
			if( bucket.Entries[ i ] == handle )
			{
				remove_at = i;
				break;
			}
		}

		DD_ASSERT( remove_at != -1 );

		int entry_count = bucket.Entries.Size();

		bucket.Entries.RemoveAt( remove_at );

		DD_ASSERT( bucket.Entries.Size() == entry_count - 1 );

		bucket.Bounds = CalculateBucketBounds( bucket );

		MergeEmptyBuckets( current_index );
		std::sort( m_freeBuckets.begin(), m_freeBuckets.end() );

		m_freeEntries.push_back( handle );
		std::sort( m_freeEntries.begin(), m_freeEntries.end() );
	}

	BVHIntersection BVHTree::IntersectsRay( const Ray& ray ) const
	{
		dd::Array<size_t, 64> stack;
		stack.Add( 0 );

		BVHIntersection nearest;
		nearest.Distance = FLT_MAX;
		nearest.Handle = INVALID;

		int buckets_tested = 0;

		while( stack.Size() > 0 )
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[ stack.Pop() ];

			float bucket_distance;
			if( bucket.Bounds.IntersectsRay( ray, bucket_distance ) &&
				bucket_distance < nearest.Distance &&
				bucket_distance < ray.Length() )
			{
				if( bucket.IsLeaf() )
				{
					for( size_t e : bucket.Entries )
					{
						float distance;
						const BVHEntry& entry = m_entries[ e ];
						if( entry.Bounds.IntersectsRay( ray, distance ) &&
							distance < nearest.Distance )
						{
							nearest.Handle = e;
							nearest.Distance = distance;
						}
					}
				}
				else
				{
					stack.Add( bucket.Left );
					stack.Add( bucket.Right );
				}
			}
		}

		//DD_DIAGNOSTIC( "[BVHTree] IntersectsRay - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return nearest;
	}

	BVHIntersection BVHTree::IntersectsRayFn( const Ray& ray, const HitTestFn& fn ) const
	{
		dd::Array<size_t, 64> stack;
		stack.Add( 0 );

		BVHIntersection nearest;
		nearest.Distance = FLT_MAX;
		nearest.Handle = INVALID;

		int buckets_tested = 0;

		while( stack.Size() > 0 )
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];

			float bucket_distance;
			if( bucket.Bounds.IntersectsRay( ray, bucket_distance ) &&
				bucket_distance < nearest.Distance && 
				bucket_distance < ray.Length() )
			{
				if( bucket.IsLeaf() )
				{
					for( size_t e : bucket.Entries )
					{
						float entry_distance;
						if( m_entries[e].Bounds.IntersectsRay( ray, entry_distance ) &&
							entry_distance < nearest.Distance )
						{
							float actual_distance = fn( e );
							if( actual_distance < nearest.Distance )
							{
								nearest.Handle = e;
								nearest.Distance = actual_distance;
							}
						}
					}
				}
				else
				{
					stack.Add( bucket.Left );
					stack.Add( bucket.Right );
				}
			}
		}

		//DD_DIAGNOSTIC( "[BVHTree] IntersectsRay - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return nearest;
	}

	bool BVHTree::WithinBounds( const AABB& bounds, std::vector<size_t>& out_hits ) const
	{
		int buckets_tested = 0;

		dd::Array<size_t, 64> stack;
		stack.Add( 0 );

		bool hit = false;

		while( stack.Size() > 0 )
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[ stack.Pop() ];

			if( bucket.Bounds.Intersects( bounds ) )
			{
				if( bucket.IsLeaf() )
				{
					for( size_t e : bucket.Entries )
					{
						if( m_entries[ e ].Bounds.Intersects( bounds ) )
						{
							out_hits.push_back( e );
							hit = true;
						}
					}
				}
				else
				{
					stack.Add( bucket.Left );
					stack.Add( bucket.Right );
				}
			}
		}

		//DD_DIAGNOSTIC( "[BVHTree] WithinBounds - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return hit;
	}

	bool BVHTree::WithinBounds( const Sphere& sphere, std::vector<size_t>& out_hits ) const
	{
		int buckets_tested = 0;

		dd::Array<size_t, 64> stack;
		stack.Add( 0 );

		bool hit = false;

		while( stack.Size() > 0 )
		{
			++buckets_tested;

			const BVHBucket& bucket = m_buckets[stack.Pop()];
			if( bucket.Bounds.Intersects( sphere ) )
			{
				if( bucket.IsLeaf() )
				{
					for( size_t e : bucket.Entries )
					{
						if( m_entries[e].Bounds.Intersects( sphere ) )
						{
							out_hits.push_back( e );
							hit = true;
						}
					}
				}
				else
				{
					stack.Add( bucket.Left );
					stack.Add( bucket.Right );
				}
			}
		}

		//DD_DIAGNOSTIC( "[BVHTree] WithinBounds - Buckets Used: %d/%zu\n", buckets_tested, m_buckets.size() );

		return hit;
	}

	void BVHTree::SplitBucket( size_t parent_index )
	{
		if( m_freeBuckets.empty() )
		{
			m_buckets.reserve( m_buckets.size() + 2 );

			m_freeBuckets.push_back( m_buckets.size() );
			m_buckets.push_back( BVHBucket() );

			m_freeBuckets.push_back( m_buckets.size() );
			m_buckets.push_back( BVHBucket() );
		}

		BVHBucket& parent_bucket = m_buckets[ parent_index ];

		DD_ASSERT( parent_bucket.IsLeaf() );

		glm::vec3 split_offset = parent_bucket.Region.Extents();
		Axis split_axis;

		if( split_offset.z >= split_offset.x && split_offset.z >= split_offset.y )
		{
			split_offset.x = 0;
			split_offset.y = 0;
			split_offset.z *= 0.5f;

			split_axis = Axis::Z;

		}
		else if( split_offset.y >= split_offset.x && split_offset.y >= split_offset.z )
		{
			split_offset.x = 0;
			split_offset.y *= 0.5f;
			split_offset.z = 0;

			split_axis = Axis::Y;
		}
		else
		{
			split_offset.x *= 0.5f;
			split_offset.y = 0;
			split_offset.z = 0;

			split_axis = Axis::X;
		}

		size_t left_index = dd::pop_front( m_freeBuckets );
		BVHBucket& left = m_buckets[ left_index ];
		left.Region.Min = parent_bucket.Region.Min;
		left.Region.Max = parent_bucket.Region.Max - split_offset;
		left.SplitAxis = split_axis;
		left.Parent = parent_index;

		size_t right_index = dd::pop_front( m_freeBuckets );
		BVHBucket& right = m_buckets[ right_index ];
		right.Region.Min = parent_bucket.Region.Min + split_offset;
		right.Region.Max = parent_bucket.Region.Max;
		right.SplitAxis = split_axis;
		right.Parent = parent_index;

		DD_ASSERT( left.Region.Volume() > 0 );
		DD_ASSERT( right.Region.Volume() > 0 );

		parent_bucket.Left = left_index;
		parent_bucket.Right = right_index;

		// split the current entries between the two buckets
		for( size_t entry_index : parent_bucket.Entries )
		{
			const BVHEntry& entry = m_entries[ entry_index ];
			glm::vec3 center = entry.Bounds.Center();

			if( left.Region.Contains( center ) )
			{
				left.Entries.Add( entry_index );
				left.Bounds.Expand( entry.Bounds );
			}
			else
			{
				right.Entries.Add( entry_index );
				right.Bounds.Expand( entry.Bounds );
			}
		}

		parent_bucket.Entries.Clear();
	}

	void BVHTree::InsertEntry( size_t entry_index )
	{
		const BVHEntry& entry = m_entries[ entry_index ];

		glm::vec3 center = entry.Bounds.Center();

		size_t current_index = 0;

		while( true )
		{
			BVHBucket& current = m_buckets[ current_index ];
			current.Bounds.Expand( entry.Bounds );

			if( current.IsLeaf() )
			{
				if( current.Entries.Size() < MAX_ENTRIES )
				{
					m_buckets[ current_index ].Entries.Add( entry_index );
					break;
				}
				else
				{
					// split and re-loop with the current bucket no longer being a leaf
					SplitBucket( current_index );
				}
			}
			else
			{
				BVHBucket& left = m_buckets[ current.Left ];
				if( left.Region.Contains( center ) )
				{
					current_index = current.Left;
				}
				else
				{
					current_index = current.Right;
				}
			}
		}
	}

	void BVHTree::RebuildTree()
	{
		m_buckets[ 0 ] = BVHBucket();
		BVHBucket& root = m_buckets[ 0 ];

		// calculate new root bounds
		for( size_t e = 0; e < m_entries.size(); ++e )
		{
			if( IsFreeEntry( e ) )
				continue;

			root.Region.Expand( m_entries[ e ].Bounds );
		}

		root.Bounds = root.Region;

		// free all buckets except root
		m_freeBuckets.clear();

		for( size_t b = 1; b < m_buckets.size(); ++b )
		{
			m_buckets[ b ] = BVHBucket();

			m_freeBuckets.push_back( b );
		}

		// insert all entries
		for( size_t e = 0; e < m_entries.size(); ++e )
		{
			if( IsFreeEntry( e ) )
				continue;

			InsertEntry( e );
		}

		++m_rebuildCount;
	}

	bool BVHTree::IsFreeEntry( size_t entry_index ) const
	{
		DD_ASSERT( entry_index < m_entries.size() );

		return std::binary_search( m_freeEntries.begin(), m_freeEntries.end(), entry_index );
	}

	bool BVHTree::IsFreeBucket( size_t bucket_index ) const
	{
		DD_ASSERT( bucket_index < m_buckets.size() );

		return std::binary_search( m_freeBuckets.begin(), m_freeBuckets.end(), bucket_index );
	}

	AABB BVHTree::CalculateBucketBounds( const BVHBucket& bucket ) const
	{
		AABB bounds;

		for( size_t e : bucket.Entries )
		{
			bounds.Expand( m_entries[ e ].Bounds );
		}

		return bounds;
	}

	void BVHTree::CountBucketSplits( int& x, int& y, int& z ) const
	{
		x = y = z = 0;

		for( const BVHBucket& bucket : m_buckets )
		{
			if( bucket.SplitAxis == Axis::X )
			{
				++x;
			}
			else if( bucket.SplitAxis == Axis::Y )
			{
				++y;
			}
			else
			{
				++z;
			}
		}
	}

	void BVHTree::EnsureAllBucketsEmpty() const
	{
		for( const BVHBucket& bucket : m_buckets )
		{
			DD_ASSERT( bucket.IsLeaf() );
			DD_ASSERT( bucket.Entries.Size() == 0 );
		}
	}
}