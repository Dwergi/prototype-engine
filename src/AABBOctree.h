//
// AABBOctree.h - An octree for storing AABB's. Each AABB may be present in multiple nodes.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

#include "ddm/AABB.h"

namespace dd
{
	// an octree entry is a handle that defines the entry, which is used to remove data from the tree
	typedef int OctreeEntry;

	class AABBOctree
	{
	public:
		AABBOctree();
		~AABBOctree();

		//
		// Reset the tree to its initial state.
		//
		void Clear();

		//
		// Add a point to the tree.
		//
		OctreeEntry Add( const glm::vec3& pt );

		//
		// Add an entry to the octree. Returns a handle for removing it in the future.
		//
		OctreeEntry Add( const ddm::AABB& bounds );

		//
		// Remove the given entry from the octree.
		//
		void Remove( OctreeEntry entry );

		//
		// Access the bounds that are used for this entry.
		//
		const ddm::AABB& GetEntry( OctreeEntry entry ) const;

		//
		// Is the given entry still valid? Only false if the entry has been deleted, or just made up.
		//
		bool IsValid( OctreeEntry entry ) const;

		//
		// Get all entries containing the given point. Returns the number of entries found for convenience.
		//
		int GetAllContaining( const glm::vec3& pt, Vector<OctreeEntry>& outResults ) const;

		//
		// Get all entries that intersect with the given bounds.
		//
		int GetAllIntersecting( const ddm::AABB& bounds, Vector<OctreeEntry>& outResults ) const;

		//
		// Get the bounds of the tree.
		//
		const ddm::AABB& GetBounds() const { return m_treeBounds; }

		// 
		// Get the internal node count of the tree.
		// Really just a diagnostic.
		// 
		int GetNodeCount() const { return m_nodes.Size(); }

		//
		// Get the number of entries in this tree.
		//
		int GetEntryCount() const { return m_entries.Size() - m_free.Size(); }

	private:

		typedef int NodeHandle;

		struct Node
		{
			NodeHandle m_children { -1 };
			Vector<OctreeEntry> m_data;

			Node();
		};

		ddm::AABB m_treeBounds;
		Vector<Node> m_nodes;
		Vector<ddm::AABB> m_entries;
		Vector<OctreeEntry> m_free;

		bool IsFull( NodeHandle node ) const;
		bool HasChildren( NodeHandle handle ) const;
		NodeHandle GetRoot() const;
		NodeHandle GetChild( NodeHandle handle, int index ) const;
		Node* GetNodePtr( NodeHandle handle ) const;

		NodeHandle CreateNode();

		void Rebuild();
		
		void CreateChildrenFor( NodeHandle handle, const ddm::AABB& bounds, int depth );
		void AddToNode( OctreeEntry entry_handle, const ddm::AABB& entry_bounds, NodeHandle node_handle, const ddm::AABB& node_bounds, int depth );
		void AddToChildren( OctreeEntry entry_handle, const ddm::AABB& entry_bounds, NodeHandle parent_handle, const ddm::AABB& parent_bounds, int depth );

		void GetContaining( const glm::vec3& pt, NodeHandle node_handle, const ddm::AABB& node_bounds, Vector<OctreeEntry>& outResults ) const;
		void GetIntersecting( const ddm::AABB& bounds, NodeHandle node_handle, const ddm::AABB& node_bounds, Vector<OctreeEntry>& outResults ) const;
	};
}
