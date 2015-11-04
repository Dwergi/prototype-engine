Node
//
// Octree2.cpp - A better octree implementation?
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "Octree2.h"

#include "AABB.h"

namespace dd
{
	const float DEFAULT_SIZE = 1;
	const float MAX_ENTRIES = 8;
	const int INVALID_NODE_HANDLE = -1;

	Octree2::Node::Node()
	{

	}

	Octree2::Octree2()
	{
	}

	Octree2::~Octree2()
	{
	}

	//
	// Add an entry with the given bounds to the tree.
	//
	OctreeEntry Octree2::Add( const AABB& bounds )
	{
		OctreeEntry entry = -1;
		if( m_free.Size() > 0 )
		{
			entry = m_free[0];
			m_free.Remove( 0 );
			m_entries[entry] = bounds;
		}
		else
		{
			entry = OctreeEntry( m_entries.Size() );
			m_entries.Add( bounds );
		}

		// check if we can fit it into the tree without expanding
		if( m_treeBounds.Contains( bounds ) )
		{
			AddToNode( entry, bounds, GetRoot(), m_treeBounds );
		}
		else
		{
			// we can't, and since our bounds are implicit, we need to rebuild everything
			Rebuild();
		}

		return entry;
	}

	//
	// Remove an entry. 
	// This is a wee bit tricky, since entries are just indices into the entries vector, and they can exist in multiple nodes.
	//
	void Octree2::Remove( OctreeEntry entry )
	{
		ASSERT( IsValid( entry ) );

		m_free.Push( entry );
		memset( &m_entries[entry], 0xffffffff, sizeof( AABB ) );

		for( Node& node : m_nodes )
		{
			int index = node.m_entries.Find( entry );
			if( index >= 0 )
			{
				node.m_entries.Remove( index );
			}
		}
	}

	Octree2::NodeHandle Octree2::CreateNode()
	{
		NodeHandle res = (int) m_nodes.Size();
		Node& node = m_nodes.Allocate();
		node.m_children = INVALID_NODE_HANDLE;
		return res;
	}

	//
	// Create all the child nodes for the given node.
	//
	void Octree2::CreateChildrenFor( Octree2::NodeHandle handle, const AABB& bounds )
	{
		{
			Node* ptr = GetNodePtr( handle );
			ptr->m_children = (int) m_nodes.Size();
		}

		for( uint i = 0; i < 8; ++i )
		{
			CreateNode();
		}

		// need to grab the node pointer again, because we might have caused a reallocation
		Node* ptr = GetNodePtr( handle );
		for( const OctreeEntry& entry : ptr->m_entries )
		{
			AddToChildren( entry, GetEntry( entry ), handle, bounds );
		}
	}

	bool Octree2::HasChildren( NodeHandle node ) const
	{
		return GetNodePtr( node )->m_children != INVALID_NODE_HANDLE;
	}

	bool Octree2::IsFull( NodeHandle node ) const
	{
		return GetNodePtr( node )->m_entries.Size() >= MAX_ENTRIES;
	}

	bool Octree2::IsValid( OctreeEntry entry ) const
	{
		if( entry < 0 || entry >= (int) m_entries.Size() )
			return false;

		if( m_free.Contains( entry ) )
			return false;

		return true;
	}

	//
	// Get the root node.
	//
	Octree2::NodeHandle Octree2::GetRoot() const
	{
		return m_nodes.Size() > 0 ? 0 : -1;
	}
	
	//
	// Get a pointer to the given node. 
	// WARNING: Do not store this pointer beyond the scope of any function call, because it will very likely be invalid after.
	//
	Octree2::Node* Octree2::GetNodePtr( Octree2::NodeHandle handle ) const
	{
		ASSERT( handle >= 0 && handle < (int) m_nodes.Size() );

		return &(m_nodes[handle]);
	}

	Octree2::NodeHandle Octree2::GetChild( Octree2::NodeHandle handle, uint index ) const
	{
		ASSERT( handle + index >= 0 && handle + index < m_nodes.Size() );

		return NodeHandle( GetNodePtr( handle )->m_children + index );
	}

	const AABB& Octree2::GetEntry( OctreeEntry entry ) const
	{
		ASSERT( entry >= 0 && entry < (int) m_entries.Size() );

		return m_entries[entry];
	}

	AABB CalculateRequiredSize( Vector<AABB>& entries )
	{
		AABB bounds;
		bounds.Min = glm::vec3( -DEFAULT_SIZE );
		bounds.Max = glm::vec3( DEFAULT_SIZE );

		for( const AABB& entry : entries )
		{
			while( glm::any( glm::lessThan( entry.Min, bounds.Min ) )
				|| glm::any( glm::greaterThan( entry.Max, bounds.Max ) ) )
			{
				bounds.Min *= 2;
				bounds.Max *= 2;
			}
		}

		return bounds;
	}

	//
	// Add a node to the 
	//
	void Octree2::AddToChildren( OctreeEntry entry_handle, const AABB& entry_bounds, Octree2::NodeHandle parent_handle, const AABB& parent_bounds )
	{
		glm::vec3 parent_center = parent_bounds.Center();
		
		glm::vec3 child_extents = parent_bounds.Extents() * 0.5f;

		ASSERT( HasChildren( parent_handle ) );

		for( uint i = 0; i < 8; ++i )
		{
			AABB child_bounds;
			child_bounds.Min = parent_bounds.Min;

			if( i & 0x4 )
			{
				child_bounds.Min.x = parent_center.x;
			}

			if( i & 0x2 )
			{
				child_bounds.Min.y = parent_center.y;
			}

			if( i & 0x1 )
			{
				child_bounds.Min.z = parent_center.z;
			}

			child_bounds.Max = child_bounds.Min + child_extents;

			if( child_bounds.Intersects( entry_bounds ) )
			{
				// found our child
				{
					AddToNode( entry_handle, entry_bounds, GetChild( parent_handle, i ), child_bounds );
				}
			}
		}
	}

	//
	// Add the given entry to the given node or one of its children. 
	// It is assumed that a test has already been done to see that this node intersects with the given entry.
	//
	void Octree2::AddToNode( OctreeEntry entry_handle, const AABB& entry_bounds, Octree2::NodeHandle node_handle, const AABB& node_bounds )
	{
		ASSERT( entry_bounds.Intersects( node_bounds ) );

		if( !HasChildren( node_handle ) )
		{
			// no children
			if( IsFull( node_handle ) )
			{
				// data list full, create children
				CreateChildrenFor( node_handle, node_bounds );

				AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds );

				// clear the entry list so that 
				GetNodePtr( node_handle )->m_entries.Clear();
			}
			else
			{
				// child list isn't full, easy life
				GetNodePtr( node_handle )->m_entries.Add( entry_handle );
			}
		}
		else
		{
			AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds );
		}
	}

	//
	// Recursively all intersecting entries from this node or its children, and put them into outResults.
	//
	void Octree2::GetIntersecting( const glm::vec3& pt, Octree2::NodeHandle node_handle, const AABB& node_bounds, Vector<AABB>& outResults ) const
	{
		if( !HasChildren( node_handle ) )
		{
			Node* node = GetNodePtr( node_handle );
			for( const OctreeEntry& entry_handle : node->m_entries )
			{
				const AABB& entry = GetEntry( entry_handle );
				if( entry.Contains( pt ) )
				{
					outResults.Add( entry );
				}
			}
		}
		else
		{
			glm::vec3 node_center = node_bounds.Center();
			glm::vec3 child_extents = node_bounds.Extents() * 0.5f;

			for( uint i = 0; i < 8; ++i )
			{
				AABB child_bounds;
				child_bounds.Min = node_bounds.Min;

				if( i & 0x4 )
				{
					child_bounds.Min.x = node_center.x;
				}

				if( i & 0x2 )
				{
					child_bounds.Min.y = node_center.y;
				}

				if( i & 0x1 )
				{
					child_bounds.Min.z = node_center.z;
				}

				child_bounds.Max = child_bounds.Min + child_extents;

				if( child_bounds.Contains( pt ) )
				{
					GetIntersecting( pt, GetChild( node_handle, i ), child_bounds, outResults );
				}
			}
		}
	}

	//
	// Get all entries that contain the given point.
	//
	uint Octree2::GetAllContaining( const glm::vec3& pt, Vector<AABB>& outResults ) const
	{
		outResults.Clear();

		// check if we intersect with the tree at all
		if( !m_treeBounds.Contains( pt ) )
			return 0;

		GetIntersecting( pt, GetRoot(), m_treeBounds, outResults );

		return outResults.Size();
	}
	
	//
	// Rebuild the entire octree from scratch, based on the entries we have.
	//
	void Octree2::Rebuild()
	{
		m_nodes.Clear();

		NodeHandle root = CreateNode();

		m_treeBounds = CalculateRequiredSize( m_entries );

		for( uint i = 0; i < m_entries.Size(); ++i )
		{
			AddToNode( OctreeEntry( i ), m_entries[ i ], root, m_treeBounds );
		}
	}
}