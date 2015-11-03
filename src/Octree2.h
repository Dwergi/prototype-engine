//
// Octree2.h - A better octree implementation, hopefully?
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

#include "AABB.h"

namespace dd
{
	struct AABB;

	// an octree entry is a handle that defines the entry, which is used to remove data from the tree
	typedef int OctreeEntry;

	class Octree2
	{
	public:
		Octree2();
		~Octree2();

		OctreeEntry Add( const AABB& bounds );
		void Remove( OctreeEntry entry );

		const AABB& GetBounds() const { return m_treeBounds; }
		uint GetNodeCount() const { return m_nodes.Size(); }
		uint GetEntryCount() const { return m_entries.Size(); }

	private:

		typedef int NodeHandle;

		struct Node
		{
			NodeHandle m_children;
			Vector<OctreeEntry> m_entries;
		};

		AABB m_treeBounds;
		Vector<Node> m_nodes;
		Vector<AABB> m_entries;

		Node* GetChild( NodeHandle handle, uint index );
		Node* GetNodePtr( NodeHandle handle );
		void CreateChildrenFor( NodeHandle handle, const AABB& bounds );

		void AddToNode( OctreeEntry entry_handle, const AABB& entry_bounds, NodeHandle node_handle, const AABB& node_bounds );
		void AddToChildren( OctreeEntry entry_handle, const AABB& entry_bounds, NodeHandle parent_handle, const AABB& parent_bounds );
		
		void Rebuild();
		NodeHandle CreateNode();
	};
}
