//
// Octree.h - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//
#pragma once

#include "Handle.h"
#include "BoundingBox.h"

#include <queue>
#include <functional>

namespace dd
{
	class Octree
	{
	private:

		class CellInternal;

	public:

		class Entry;

		Octree();
		~Octree();

		// 
		// Add an entry at the given position. 
		// Returns a handle used to reference the given object.
		//
		Octree::Entry Add( const Vector4& pos );

		//
		// Remove the given entry.
		//
		void Remove( const Octree::Entry& entry );

		//
		// Get the <count> nearest entries to the given handle.
		// 
		void GetKNearest( const Octree::Entry& entry, int count, std::vector<Octree::Entry>& output );

		//
		// Get all the entries within <range> of the given handle.
		//
		void GetWithinRange( const Octree::Entry& entry, float range, std::vector<Octree::Entry>& output );

		//
		// Get all the entries within the given (axis-aligned) bounding box.
		//
		void GetWithinBounds( const BoundingBox& bounds, std::vector<Octree::Entry>& output );

		// 
		// Validate that the octree is well-formed.
		//
		void Validate() const;

		//
		// Get the position associated with the given entry.
		//
		const dd::Vector4& GetPosition( const Octree::Entry& entry ) const;
	
		//
		// Gets the number of entries in the tree.
		//
		int Count() const;

		//
		// An entry in the octree. Used to reference entries in the tree.
		// 
		class Entry
		{
		private:
			friend class Octree;
			friend struct hash;

			int ID;
			Entry( int id );

		public:
			Entry();
			Entry( const Entry& other );
			bool operator==( const Entry& other ) const;
			bool IsValid() const;

			struct hash
			{
				std::size_t operator()( const Entry& entry ) const
				{
					return entry.ID;
				}
			};
		};

	private:

		typedef Handle<CellInternal, int, std::vector<CellInternal>> Cell;
		friend class Cell;

		//
		// Internal cell implementation.
		// 
		class CellInternal
		{
		public:
			Vector4 Position;
			float Size;
			Octree::Cell Children[ 8 ];
			std::vector<Octree::Entry> Data;

			CellInternal( const Vector4& pos, float size );
			CellInternal( const CellInternal& other );
			~CellInternal();
			CellInternal& operator=( const CellInternal& other );

			void Add( const Octree::Entry& entry );
			void Remove( const Octree::Entry& entry );

			bool Contains( const Vector4& pos ) const;
			Octree::Cell GetCellContaining( const Vector4& pos ) const;
			bool HasChildren() const;
			BoundingBox GetBounds() const;
		};

		// Map of entry IDs to positions.
		std::unordered_map<Entry, Vector4, Entry::hash> m_entries;

		// List of all existing cells.
		std::vector<Octree::CellInternal> m_cells;

		// The root cell.
		Octree::Cell m_root;
	
		// List of free cells.
		std::queue<Cell> m_free;

		// The last created ID.
		int m_lastID;

		// Split a cell into 8 sub-cells.
		void SplitCell( Octree::Cell cell );

		// Expand the root node of the octree to encompass more space.
		void ExpandRoot();

		// Create the children for the given cell.
		void CreateChildren( Octree::Cell cell );
		Octree::Cell CreateCell( const Vector4& position, float size );
		void AddToCell( Octree::Cell cell, const Octree::Entry& entry );

		void FreeCell( Octree::Cell cell );

		Octree::Cell FindCell( const Vector4& position, Octree::Cell* parent = nullptr ) const;
		Octree::Cell FindCell( const Octree::Entry& position, Octree::Cell* parent = nullptr ) const;

		Octree::Entry NextEntry();
	};
}