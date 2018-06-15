//
// BVHTree.h - Bounding volume hierarchy for AABBs. 
// Copyright (C) Sebastian Nordgren 
// June 15th 2018
//

#include "PrecompiledHeader.h"
#include "BVHTree.h"

#include "AABB.h"

#include <algorithm>

namespace std
{
	template <typename T>
	T pop_front( std::vector<T>& vec )
	{
		T value = vec.front();
		vec.erase( vec.begin() );

		return value;
	}
}

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

		size_t index = std::pop_front( m_freeEntries );

		BVHEntry& entry = m_entries[ index ];
		entry.Handle = index;
		entry.Bounds = bounds;

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

		return entry.Handle;
	}

	void BVHTree::Remove( int handle )
	{
		if( IsFreeEntry( handle ) )
		{
			return;
		}

		BVHEntry& entry = m_entries[ handle ];
		glm::vec3 center = entry.Bounds.Center();

		// find the bucket
		size_t current_index = 0;
		size_t parent_index = INVALID;
		while( true )
		{
			BVHBucket& current = m_buckets[ current_index ];
			if( current.IsLeaf() )
			{
				break;
			}
			
			parent_index = current_index;

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

		if( remove_at != -1 )
		{
			bucket.Entries.RemoveAt( remove_at );
			bucket.Bounds = CalculateBucketBounds( bucket );
		}

		// merge buckets into parent if possible
		if( parent_index != INVALID )
		{
			BVHBucket& parent = m_buckets[ parent_index ];

			if( m_buckets[ parent.Left ].Entries.Size() == 0 &&
				m_buckets[ parent.Right ].Entries.Size() == 0 )
			{
				// both children empty
				parent.Left = INVALID;
				parent.Right = INVALID;

				m_freeBuckets.push_back( parent.Left );
				m_freeBuckets.push_back( parent.Right );

				std::sort( m_freeBuckets.begin(), m_freeBuckets.end() );

				DD_TODO( "Could merge the parent with its parent as well..." );
			}
		}

		m_freeEntries.push_back( handle );
		std::sort( m_freeEntries.begin(), m_freeEntries.end() );
	}

	bool BVHTree::IntersectsRay( const Ray& ray, std::vector<Intersection>& outResults ) const
	{
		dd::Array<size_t, 64> stack;
		stack.Add( 0 );

		while( stack.Size() > 0 )
		{
			BVHBucket& bucket = m_buckets[ stack[ stack.Size() -1 ] ];
			stack.RemoveAt( stack.Size() - 1 );

			if( bucket.Bounds.IntersectsRay( ray, ignored ) )
			{
				if( bucket.IsLeaf() )
				{
					for( size_t e = 0; e < bucket.Entries.Size(); ++e )
					{
						float distance = 0;
						if( m_entries[ e ].Bounds.IntersectsRay( ray, distance ) )
						{
							Intersection i;
							i.Handle = m_entries[ e ].Handle;
							i.Distance = distance;

							outResults.push_back( i );
						}
					}
				}
				else
				{
					float ignored;
					if( m_buckets[ bucket.Left ].Bounds.IntersectsRay( ray, ignored ) )
					{
						stack.Add( bucket.Left );
					}
					
					if( m_buckets[ bucket.Right ].Bounds.IntersectsRay( ray, ignored ) )
					{
						stack.Add( bucket.Right );
					}
				}
			}
		}
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

		if( split_offset.z > split_offset.x && split_offset.z > split_offset.y )
		{
			split_offset.x = 0;
			split_offset.y = 0;
			split_offset.z *= 0.5f;

		}
		else if( split_offset.y > split_offset.x && split_offset.y > split_offset.z )
		{
			split_offset.x = 0;
			split_offset.y *= 0.5f;
			split_offset.z = 0;
		}
		else
		{
			split_offset.x *= 0.5f;
			split_offset.y = 0;
			split_offset.z = 0;
		}

		size_t left_index = std::pop_front( m_freeBuckets );
		BVHBucket& left = m_buckets[ left_index ];
		left.Region.Min = parent_bucket.Region.Min;
		left.Region.Max = parent_bucket.Region.Max - split_offset;

		size_t right_index = std::pop_front( m_freeBuckets );
		BVHBucket& right = m_buckets[ right_index ];
		right.Region.Min = parent_bucket.Region.Min + split_offset;
		right.Region.Max = parent_bucket.Region.Max;

		DD_ASSERT( !left.Region.Intersects( right.Region ) );
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
				left.Entries.Add( entry.Handle );
				left.Bounds.Expand( entry.Bounds );
			}
			else
			{
				right.Entries.Add( entry.Handle );
				right.Bounds.Expand( entry.Bounds );
			}
		}
	}

	void BVHTree::InsertEntry( size_t entry_index )
	{
		const BVHEntry& entry = m_entries[ entry_index ];

		glm::vec3 center = entry.Bounds.Center();

		size_t current_index = 0;
		int iterations = 0;

		while( true )
		{
			DD_ASSERT( iterations < 30 );

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

			++iterations;
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
}