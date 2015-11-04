//
// AABBOctree.h - An octree for storing AABB's. Each AABB may be present in multiple nodes.
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

	class AABBOctree
	{
	public:
		AABBOctree();
		~AABBOctree();

		//
		// Add an entry to the octree. Returns a handle for removing it in the future.
		//
		OctreeEntry Add( const AABB& bounds );

		//
		// Remove the given entry from the octree.
		//
		void Remove( OctreeEntry entry );

		//
		// Access the bounds that are used for this entry.
		//
		const AABB& GetEntry( OctreeEntry entry ) const;

		//
		// Is the given entry still valid? Only false if the entry has been deleted, or just made up.
		//
		bool IsValid( OctreeEntry entry ) const;

		uint GetAllContaining( const glm::vec3& pt, Vector<OctreeEntry>& outResults ) const;

		const AABB& GetBounds() const { return m_treeBounds; }
		uint GetNodeCount() const { return m_nodes.Size(); }
		uint GetEntryCount() const { return m_entries.Size() - m_free.Size(); }

	private:

		typedef int NodeHandle;

		struct Node
		{
			NodeHandle m_children;
			Vector<OctreeEntry> m_entries;

			Node();
		};

		AABB m_treeBounds;
		Vector<Node> m_nodes;
		Vector<AABB> m_entries;
		Vector<OctreeEntry> m_free;

		bool IsFull( NodeHandle node ) const;
		bool HasChildren( NodeHandle handle ) const;
		NodeHandle GetChild( NodeHandle handle, uint index ) const;
		Node* GetNodePtr( NodeHandle handle ) const;
		NodeHandle GetRoot() const;

		NodeHandle CreateNode();
		void CreateChildrenFor( NodeHandle handle, const AABB& bounds, uint depth );
		void AddToNode( OctreeEntry entry_handle, const AABB& entry_bounds, NodeHandle node_handle, const AABB& node_bounds, uint depth );
		void AddToChildren( OctreeEntry entry_handle, const AABB& entry_bounds, NodeHandle parent_handle, const AABB& parent_bounds, uint depth );
		
		void Rebuild();

		void GetIntersecting( const glm::vec3& pt, NodeHandle node_handle, const AABB& node_bounds, Vector<OctreeEntry>& outResults ) const;
	};
}
