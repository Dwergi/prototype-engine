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

	Octree2::Octree2()
	{
	}

	Octree2::~Octree2()
	{
	}

	OctreeEntry Octree2::Add( const AABB& bounds )
	{
		OctreeEntry entry = OctreeEntry( m_entries.Size() );
		m_entries.Add( bounds );

		Rebuild();

		return entry;
	}

	Octree2::NodeHandle Octree2::CreateNode()
	{
		NodeHandle res = (int) m_nodes.Size();
		Node& node = m_nodes.Allocate();
		node.m_children = INVALID_NODE_HANDLE;
		return res;
	}

	void Octree2::CreateChildrenFor( Octree2::NodeHandle handle, const AABB& bounds )
	{
		Node* ptr = GetNodePtr( handle );

		ptr->m_children = (int) m_nodes.Size();

		for( uint i = 0; i < 8; ++i )
		{
			// reserve 8 children in the node list
			m_nodes.Allocate();
		}

		for( OctreeEntry entry : ptr->m_entries )
		{
			AddToChildren( entry, m_entries[entry], handle, bounds );
		}
	}
	
	Octree2::Node* Octree2::GetNodePtr( Octree2::NodeHandle handle )
	{
		return &m_nodes[handle];
	}

	Octree2::Node* Octree2::GetChild( Octree2::NodeHandle handle, uint index )
	{
		return GetNodePtr( handle + index );
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

	void Octree2::AddToChildren( OctreeEntry entry_handle, const AABB& entry_bounds, Octree2::NodeHandle parent_handle, const AABB& parent_bounds )
	{
		glm::vec3 parent_center = parent_bounds.Center();
		
		glm::vec3 child_size = parent_bounds.Max - parent_bounds.Min;

		Node* node = GetNodePtr( parent_handle );
		
		ASSERT( node->m_children != INVALID_NODE_HANDLE );

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

			child_bounds.Max = child_bounds.Min + child_size;

			if( child_bounds.Intersects( entry_bounds ) )
			{
				// found our child
				AddToNode( entry_handle, entry_bounds, node->m_children + i, child_bounds );
			}
		}
	}

	void Octree2::AddToNode( OctreeEntry entry_handle, const AABB& entry_bounds, Octree2::NodeHandle node_handle, const AABB& node_bounds )
	{
		ASSERT( entry_bounds.Intersects( node_bounds ) );

		Node* node = GetNodePtr( node_handle );

		if( node->m_children == INVALID_NODE_HANDLE )
		{
			// no children
			if( node->m_entries.Size() >= MAX_ENTRIES )
			{
				// child list full, create children
				CreateChildrenFor( node_handle, node_bounds );

				AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds );
			}
			else
			{
				// child list isn't full, easy life
				node->m_entries.Add( entry_handle );
			}
		}
		else
		{
			AddToChildren( entry_handle, entry_bounds, node_handle, node_bounds );
		}
	}
	
	//
	// Rebuild the entire octree from scratch, based on the entries we have.
	//
	void Octree2::Rebuild()
	{
		m_nodes.Clear();

		NodeHandle root = CreateNode();

		// precalculate bounds
		m_treeBounds = CalculateRequiredSize( m_entries );

		for( uint i = 0; i < m_entries.Size(); ++i )
		{
			AddToNode( OctreeEntry( i ), m_entries[ i ], root, m_treeBounds );
		}
	}
}