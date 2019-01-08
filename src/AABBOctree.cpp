//
// AABBOctree.cpp - An octree for AABB's, where each AABB can be present in multiple nodes, at multiple levels of the hierarchy.
// The size of each node is implicit, defined only by the size of the tree and the current depth. 
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PCH.h"
#include "AABBOctree.h"

namespace dd
{
	const int MAX_DEPTH = 5;
	const float DEFAULT_SIZE = 1;
	const float MAX_ENTRIES = 8;
	const int INVALID_NODE_HANDLE = -1;

	AABBOctree::Node::Node()
	{

	}

	AABBOctree::AABBOctree()
	{
	}

	AABBOctree::~AABBOctree()
	{
	}

	//
	// Reset the tree to its initial state.
	//
	void AABBOctree::Clear()
	{
		m_nodes.Clear();
		m_entries.Clear();
		m_free.Clear();
		m_treeBounds = ddm::AABB {};
	}

	//
	// Add a point to the tree.
	//
	OctreeEntry AABBOctree::Add( const glm::vec3& pt )
	{
		ddm::AABB bb;
		bb.Expand( pt );
		return Add( bb );
	}

	//
	// Add an entry with the given bounds to the tree.
	//
	OctreeEntry AABBOctree::Add( const ddm::AABB& bounds )
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
			AddToNode( entry, bounds, GetRoot(), m_treeBounds, 0 );
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
	void AABBOctree::Remove( OctreeEntry entry )
	{
		DD_ASSERT( IsValid( entry ) );

		m_free.Add( entry );
		memset( &m_entries[entry], 0xFF, sizeof( ddm::AABB ) );

		for( Node& node : m_nodes )
		{
			int index = node.m_data.Find( entry );
			if( index >= 0 )
			{
				node.m_data.Remove( index );
			}
		}
	}

	//
	// Create a new node.
	//
	AABBOctree::NodeHandle AABBOctree::CreateNode()
	{
		NodeHandle res = (int) m_nodes.Size();
		Node& node = m_nodes.Allocate();
		node.m_children = INVALID_NODE_HANDLE;
		return res;
	}

	//
	// Create all the child nodes for the given node.
	//
	void AABBOctree::CreateChildrenFor( AABBOctree::NodeHandle handle, const ddm::AABB& bounds, int depth )
	{
		GetNodePtr( handle )->m_children = m_nodes.Size();

		for( int i = 0; i < 8; ++i )
		{
			CreateNode();
		}

		// need to grab the node pointer again, because we might have caused a reallocation
		Node* node = GetNodePtr( handle );
		for( const OctreeEntry& entry : node->m_data )
		{
			AddToChildren( entry, GetEntry( entry ), handle, bounds, depth );
		}

		// clear the entry list so that we don't have data and children
		GetNodePtr( handle )->m_data.Clear();
	}

	//
	// Does the given node have children?
	//
	bool AABBOctree::HasChildren( NodeHandle node ) const
	{
		return GetNodePtr( node )->m_children != INVALID_NODE_HANDLE;
	}

	//
	// Is the given node's data array full?
	//
	bool AABBOctree::IsFull( NodeHandle node ) const
	{
		return GetNodePtr( node )->m_data.Size() >= MAX_ENTRIES;
	}

	//
	// Is the given entry handle valid?
	//
	bool AABBOctree::IsValid( OctreeEntry entry ) const
	{
		if( entry < 0 || entry >= (int) m_entries.Size() )
			return false;

		if( m_free.Contains( entry ) )
			return false;

		return true;
	}

	//
	// Get the bounds of the child at the given index from a parent with the given bounds.
	//
	static ddm::AABB GetChildBounds( glm::vec3 parent_center, glm::vec3 half_extents, int index )
	{
		ddm::AABB child_bounds;
		child_bounds.Min = parent_center - half_extents;

		if( index & 0x4 )
			child_bounds.Min.x = parent_center.x;

		if( index & 0x2 )
			child_bounds.Min.y = parent_center.y;

		if( index & 0x1 )
			child_bounds.Min.z = parent_center.z;

		child_bounds.Max = child_bounds.Min + half_extents;

		return child_bounds;
	}

	//
	// Get the root node.
	//
	AABBOctree::NodeHandle AABBOctree::GetRoot() const
	{
		return m_nodes.Size() > 0 ? 0 : -1;
	}
	
	//
	// Get a pointer to the given node. 
	// WARNING: Do not store this pointer beyond the scope of any function call, because it will very likely be invalid after.
	//
	AABBOctree::Node* AABBOctree::GetNodePtr( AABBOctree::NodeHandle handle ) const
	{
		DD_ASSERT( handle >= 0 && handle < (int) m_nodes.Size() );

		return &(m_nodes[handle]);
	}

	//
	// Get the child at the given index from the parent node given.
	//
	AABBOctree::NodeHandle AABBOctree::GetChild( AABBOctree::NodeHandle handle, int index ) const
	{
		DD_ASSERT( handle + index >= 0 && handle + index < m_nodes.Size() );

		return NodeHandle( GetNodePtr( handle )->m_children + index );
	}

	//
	// Get the given entry from a handle.
	//
	const ddm::AABB& AABBOctree::GetEntry( OctreeEntry entry ) const
	{
		DD_ASSERT( entry >= 0 && entry < (int) m_entries.Size() );

		return m_entries[entry];
	}

	//
	// Calculate the required size to contain all the given entries.
	//
	ddm::AABB CalculateRequiredSize( Vector<ddm::AABB>& entries )
	{
		ddm::AABB bounds;
		bounds.Min = glm::vec3( -DEFAULT_SIZE );
		bounds.Max = glm::vec3( DEFAULT_SIZE );

		for( const ddm::AABB& entry : entries )
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
	// Add a node to the given node's children. It is assumed that the entry has been checked against the parent's bounds.
	//
	void AABBOctree::AddToChildren( OctreeEntry entry_handle, const ddm::AABB& entry_bounds, AABBOctree::NodeHandle parent_handle, const ddm::AABB& parent_bounds, int depth )
	{
		DD_ASSERT( HasChildren( parent_handle ) );
		DD_ASSERT( entry_bounds.Intersects( parent_bounds ) );

		glm::vec3 parent_center = parent_bounds.Center();
		glm::vec3 half_extents = parent_bounds.Extents() * 0.5f;

		for( int i = 0; i < 8; ++i )
		{
			ddm::AABB child_bounds = GetChildBounds( parent_center, half_extents, i );
			if( child_bounds.Intersects( entry_bounds ) )
			{
				// found our child
				AddToNode( entry_handle, entry_bounds, GetChild( parent_handle, i ), child_bounds, depth );
			}
		}
	}

	//
	// Add the given entry to the given node or one of its children. 
	// It is assumed that a test has already been done to see that this node intersects with the given entry.
	//
	void AABBOctree::AddToNode( OctreeEntry entry_handle, const ddm::AABB& entry_bounds, AABBOctree::NodeHandle node_handle, const ddm::AABB& node_bounds, int depth )
	{
		DD_ASSERT( depth <= MAX_DEPTH );
		DD_ASSERT( entry_bounds.Intersects( node_bounds ) );

		if( !HasChildren( node_handle ) )
		{
			// no children
			if( IsFull( node_handle ) && depth < MAX_DEPTH )
			{
				// data list full, create children
				CreateChildrenFor( node_handle, node_bounds, depth + 1 );

				AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds, depth + 1 );
			}
			else
			{
				// child list isn't full, easy life
				GetNodePtr( node_handle )->m_data.Add( entry_handle );
			}
		}
		else
		{
			AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds, depth + 1 );
		}
	}

	//
	// Recursively find all entries containing the given point from this node or its children, and put them into outResults.
	//
	void AABBOctree::GetContaining( const glm::vec3& pt, AABBOctree::NodeHandle node_handle, const ddm::AABB& node_bounds, Vector<OctreeEntry>& outResults ) const
	{
		if( !HasChildren( node_handle ) )
		{
			Node* node = GetNodePtr( node_handle );
			for( const OctreeEntry& entry_handle : node->m_data )
			{
				const ddm::AABB& entry = GetEntry( entry_handle );
				if( entry.Contains( pt ) )
				{
					outResults.Add( entry_handle );
				}
			}
		}
		else
		{
			glm::vec3 parent_center = node_bounds.Center();
			glm::vec3 half_extents = node_bounds.Extents() * 0.5f;

			for( int i = 0; i < 8; ++i )
			{
				ddm::AABB child_bounds = GetChildBounds( parent_center, half_extents, i );
				if( child_bounds.Contains( pt ) )
				{
					GetContaining( pt, GetChild( node_handle, i ), child_bounds, outResults );
				}
			}
		}
	}

	//
	// Get all entries that contain the given point.
	//
	int AABBOctree::GetAllContaining( const glm::vec3& pt, Vector<OctreeEntry>& outResults ) const
	{
		outResults.Clear();

		// check if we intersect with the tree at all
		if( !m_treeBounds.Contains( pt ) )
			return 0;

		Vector<OctreeEntry> entries;
		GetContaining( pt, GetRoot(), m_treeBounds, entries );

		// sort, then filter out unique entries
		std::sort( entries.begin(), entries.end() );

		OctreeEntry last = -1;
		for( const OctreeEntry& entry : entries )
		{
			if( entry != last )
			{
				outResults.Add( entry );

				last = entry;
			}
		}

		return outResults.Size();
	}

	//
	// Recursively find all intersecting entries from this node or its children, and put them into outResults.
	//
	void AABBOctree::GetIntersecting( const ddm::AABB& bounds, AABBOctree::NodeHandle node_handle, const ddm::AABB& node_bounds, Vector<OctreeEntry>& outResults ) const
	{
		if( !HasChildren( node_handle ) )
		{
			Node* node = GetNodePtr( node_handle );
			for( const OctreeEntry& entry_handle : node->m_data )
			{
				const ddm::AABB& entry = GetEntry( entry_handle );
				if( entry.Intersects( bounds ) )
				{
					outResults.Add( entry_handle );
				}
			}
		}
		else
		{
			glm::vec3 parent_center = node_bounds.Center();
			glm::vec3 half_extents = node_bounds.Extents() * 0.5f;

			for( int i = 0; i < 8; ++i )
			{
				ddm::AABB child_bounds = GetChildBounds( parent_center, half_extents, i );
				if( child_bounds.Intersects( bounds ) )
				{
					GetIntersecting( bounds, GetChild( node_handle, i ), child_bounds, outResults );
				}
			}
		}
	}

	//
	// Get all entries that intersect with the given bounds.
	//
	int AABBOctree::GetAllIntersecting( const ddm::AABB& search_bounds, Vector<OctreeEntry>& outResults ) const
	{
		outResults.Clear();

		// check if we intersect with the tree at all
		if( !m_treeBounds.Intersects( search_bounds ) )
			return 0;

		Vector<OctreeEntry> entries;
		GetIntersecting( search_bounds, GetRoot(), m_treeBounds, entries );

		// sort, then filter out unique entries
		std::sort( entries.begin(), entries.end() );

		OctreeEntry last = -1;
		for( const OctreeEntry& entry : entries )
		{
			if( entry != last )
			{
				outResults.Add( entry );

				last = entry;
			}
		}

		return outResults.Size();
	}
	
	//
	// Rebuild the entire octree from scratch, based on the entries we have.
	//
	void AABBOctree::Rebuild()
	{
		m_nodes.Clear();

		NodeHandle root = CreateNode();

		m_treeBounds = CalculateRequiredSize( m_entries );

		for( int i = 0; i < m_entries.Size(); ++i )
		{
			AddToNode( OctreeEntry( i ), m_entries[ i ], root, m_treeBounds, 0 );
		}
	}
}