//
// Octree.h - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#pragma once

#include "Handle.h"
#include "BoundingBox.h"

#include "DenseMap.h"
#include "Vector.h"

#include <functional>

namespace dd
{
	class Octree
	{
	private:

		class Cell;

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
		void GetKNearest( const Octree::Entry& entry, uint count, dd::Vector<Octree::Entry>& output );

		//
		// Get all the entries within <range> of the given handle.
		//
		void GetWithinRange( const Octree::Entry& entry, float range, dd::Vector<Octree::Entry>& output );

		//
		// Get all the entries within the given (axis-aligned) bounding box.
		//
		void GetWithinBounds( const BoundingBox& bounds, dd::Vector<Octree::Entry>& output );

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
		uint Count() const;

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
			Entry( Entry&& other );
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

		class CellRef
		{
		public:
			CellRef();
			CellRef( const CellRef& other );
			CellRef( CellRef&& other );
			CellRef( Octree* octree, uint index );
			~CellRef();

			Cell* operator->() const;
			Cell& operator*() const;
			CellRef& operator=( const Octree::CellRef& other );

			void Invalidate();
			bool IsValid() const;
			int Index() const;

		private:
			int m_index;
			Octree* m_octree;
		};

		//
		// Internal cell implementation.
		// 
		class Cell
		{
		public:
			Vector4 Position;
			float Size;
			CellRef Children[ 8 ];
			dd::Vector<Octree::Entry> Data;

			Cell( const Vector4& pos, float size );
			Cell( const Cell& other );
			~Cell();
			Cell& operator=( const Cell& other );

			void Add( const Octree::Entry& entry );
			void Remove( const Octree::Entry& entry );

			bool Contains( const Vector4& pos ) const;
			const CellRef& GetCellContaining( const Vector4& pos ) const;
			bool HasChildren() const;
			BoundingBox GetBounds() const;
		};

		// Map of entry IDs to positions.
		dd::DenseMap<Entry, Vector4> m_entries;

		// List of all existing cells.
		dd::Vector<Octree::Cell> m_cells;

		// The root cell.
		CellRef m_root;
	
		// List of free cells.
		dd::Vector<int> m_free;

		// The last created ID.
		int m_lastID;

		// Split a cell CellRefo 8 sub-cells.
		void SplitCell( const CellRef& cell );

		// Expand the root node of the octree to encompass more space.
		void ExpandRoot();

		// Create the children for the given cell.
		void CreateChildren( const CellRef& cell );
		CellRef CreateCell( const Vector4& position, float size );
		void AddToCell( const CellRef& cell, const Octree::Entry& entry );

		void FreeCell( CellRef& cell );
		
		void MergeChildren( CellRef& cell );

		CellRef FindCell( const Vector4& position, CellRef* parent = nullptr ) const;
		CellRef FindCell( const Octree::Entry& position, CellRef* parent = nullptr ) const;

		Entry NextEntry();

		Cell& GetCell( int cell ) const;
	};
}