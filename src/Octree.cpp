//
// Octree.cpp - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"
#include "Octree.h"

#include <algorithm>

#define ORIGIN Vector4( 0, 0, 0 )

#define INVALID_CELL -1

const float EPSILON = 0.0001f;
const float DEFAULT_SIZE = 10.0f;
const int MAX_CELL_ENTRIES = 32;
const float MERGE_THRESHOLD = 0.66f;

//===================================================================================

namespace dd
{
	Octree::Entry::Entry( int id )
		: ID( id )
	{

	}

	Octree::Entry::Entry()
		: ID( -1 )
	{

	}

	Octree::Entry::Entry( Octree::Entry&& other )
		: ID( other.ID )
	{
		other.ID = -1;
	}

	Octree::Entry::Entry( const Octree::Entry& other )
		: ID( other.ID )
	{

	}

	Octree::Entry& Octree::Entry::operator=( const Entry& other )
	{
		ID = other.ID;

		return *this;
	}

	bool Octree::Entry::operator==( const Octree::Entry& other ) const
	{
		return ID == other.ID;
	}

	bool Octree::Entry::IsValid() const
	{
		return ID >= 0;
	}

	//===================================================================================

	Octree::CellRef::CellRef()
		: m_octree( nullptr ),
		m_index( -1 )
	{

	}

	Octree::CellRef::CellRef( const CellRef& other )
		: m_octree( other.m_octree ),
		m_index( other.m_index )
	{

	}

	Octree::CellRef::CellRef( CellRef&& other )
		: m_octree( other.m_octree ),
		m_index( other.m_index )
	{
		other.m_octree = nullptr;
		other.m_index = 0;
	}

	Octree::CellRef::CellRef( Octree* octree, uint index )
		: m_octree( octree ),
		m_index( (int) index )
	{
		
	}

	Octree::CellRef::~CellRef()
	{
		m_octree = nullptr;
		m_index = -1;
	}

	Octree::CellRef& Octree::CellRef::operator=( const Octree::CellRef& other )
	{
		m_octree = other.m_octree;
		m_index = other.m_index;

		return *this;
	}

	Octree::Cell* Octree::CellRef::operator->() const
	{
		ASSERT( IsValid() );

		return &m_octree->GetCell( m_index );
	}

	Octree::Cell& Octree::CellRef::operator*() const
	{
		ASSERT( IsValid() );

		return m_octree->GetCell( m_index );
	}

	bool Octree::CellRef::IsValid() const
	{
		return m_octree != nullptr && m_index >= 0;
	}

	int Octree::CellRef::Index() const
	{
		return m_index;
	}

	void Octree::CellRef::Invalidate()
	{
		m_octree = nullptr;
		m_index = -1;
	}

	//===================================================================================

	Octree::Cell::Cell( const Vector4& pos, float size )
	{
		Position = pos;
		Size = size;
	}

	Octree::Cell::Cell( const Cell& other )
		: Data( other.Data )
	{
		Position = other.Position;
		Size = other.Size;

		for( int i = 0; i < 8; ++i )
			Children[ i ] = other.Children[ i ];
	}

	Octree::Cell::~Cell()
	{
		for( int i = 0; i < 8; ++i )
			Children[ i ] = CellRef();
	}

	Octree::Cell& Octree::Cell::operator=( const Cell& other )
	{
		Data = other.Data;
		Position = other.Position;
		Size = other.Size;

		for( int i = 0; i < 8; ++i )
			Children[ i ] = other.Children[ i ];

		return *this;
	}

	void Octree::Cell::Add( const Octree::Entry& entry )
	{
		ASSERT( !HasChildren() );
		ASSERT( Data.Size() < MAX_CELL_ENTRIES );
		ASSERT( entry.IsValid() );

		int index = Data.Find( entry );

		ASSERT( index == -1 );

		Data.Push( entry );
	}

	void Octree::Cell::Remove( const Octree::Entry& entry )
	{
		ASSERT( !HasChildren() );
		ASSERT( entry.IsValid() );

		int index = Data.Find( entry );

		ASSERT( index >= 0 );
	
		Data.Remove( index );
	}

	bool Octree::Cell::Contains( const Vector4& pos ) const
	{
		return pos >= Position && pos < Position + Size;
	}

	const Octree::CellRef& Octree::Cell::GetCellContaining( const Vector4& pos ) const
	{
		int index = 0;

		float halfSize = Size / 2;
		if( pos.X >= Position.X + halfSize )
			index |= 0x4;

		if( pos.Y >= Position.Y + halfSize ) 
			index |= 0x2;

		if( pos.Z >= Position.Z + halfSize ) 
			index |= 0x1;

		return Children[ index ];
	}

	bool Octree::Cell::HasChildren() const
	{
		// can't have a single child valid without all children being valid
		return Children[ 0 ].IsValid();
	}

	BoundingBox Octree::Cell::GetBounds() const
	{
		return BoundingBox( Position, Position + Size );
	}

	//===================================================================================

	Octree::Octree()
	{
		m_lastID = 1;
	}

	Octree::~Octree()
	{
		m_cells.Clear();
	}

	// 
	// Add an entry at the given position. 
	// Returns a entry used to reference the given object.
	//
	Octree::Entry Octree::Add( const Vector4& position )
	{
		ASSERT( position >= ORIGIN );

		// no root created
		if( !m_root.IsValid() )
		{
			ASSERT( m_cells.Size() == 0 );

			m_root = CreateCell( ORIGIN, DEFAULT_SIZE );
		}

		// root is mis-sized
		while( !m_root->Contains( position ) )
		{
			if( m_entries.Size() == 0 )
			{
				m_root->Size *= 2;
			}
			else
			{
				ExpandRoot();
			}
		}

		// find the lowest level cell that contains the position
		const Octree::Entry& id = NextEntry();

		ASSERT( id.IsValid() );

		m_entries.Add( id, position );

		const CellRef& cell = FindCell( position );
		AddToCell( cell, id );

		return id;
	}

	//
	// Once we've found the current lowest level cell, we call this which performs all the potential splits that may result from the add.
	// 
	void Octree::AddToCell( const Octree::CellRef& cellRef, const Octree::Entry& entry )
	{
		ASSERT( cellRef.IsValid() );

		cellRef->Add( entry );

		// check if we need to split the cell
		if( cellRef->Data.Size() >= MAX_CELL_ENTRIES )
		{
			SplitCell( cellRef );
		}
	}

	//
	// Create a new cell at the given position with the given size.
	// 
	Octree::CellRef Octree::CreateCell( const Vector4& pos, float size )
	{
		if( m_free.Size() == 0 )
		{
			uint index = m_cells.Size();
			
			m_cells.Allocate( Cell( pos, size ) );

			return CellRef( this, (int) index );
		}
		else
		{
			int cellIndex = m_free.Pop();

			// placement new to make the cell usable
			Cell* cell = new (&m_cells[ cellIndex ]) Cell( pos, size );

			return CellRef( this, cellIndex );
		}
	}

	//
	// Double the size of the root by adding a node above it.
	//
	void Octree::ExpandRoot()
	{
		// copy the old root to the end
		CellRef oldRoot = CreateCell( m_root->Position, m_root->Size );

		*oldRoot = *m_root;
		oldRoot->Data.Clear();

		// expand the new root
		m_root->Size *= 2;

		// create new children
		CreateChildren( m_root );

		// old root is the first child of the expanded root, so replace the newly created child with it
		FreeCell( m_root->Children[ 0 ] );
		m_root->Children[ 0 ] = oldRoot;

		Vector<Entry> entries( m_root->Data );
		m_root->Data.Clear();

		for( const Entry& entry : entries )
		{
			ASSERT( entry.IsValid() );

			const Vector4& position = m_entries[ entry ];

			const CellRef& cell = FindCell( position );
			AddToCell( cell, entry );
		}

		// nothing should be placed in the root anymore
		ASSERT( m_root->Data.Size() == 0 );
	}

	void Octree::CreateChildren( const Octree::CellRef& cell )
	{
		ASSERT( cell.IsValid() );

		// create new cells
		float newSize	= cell->Size / 2;
		Vector4 middle	= cell->Position + newSize;

		// subdivide into new cells
		for( int i = 0; i < 8; ++i )
		{
			Vector4 pos = cell->Position;

			if( i & 0x4 )
				pos.X = middle.X;

			if( i & 0x2 )
				pos.Y = middle.Y;

			if( i & 0x1 )
				pos.Z = middle.Z;

			cell->Children[ i ] = CreateCell( pos, newSize ); 
		}
	}

	// 
	// Split a cell into 8 new subcells. 
	//
	void Octree::SplitCell( const Octree::CellRef& cell )
	{
		ASSERT( cell.IsValid() );

		CreateChildren( cell );

		// assign existing entries into new cells
		Vector<Entry> entries( cell->Data );
		cell->Data.Clear();

		for( const Entry& entry : entries )
		{
			ASSERT( entry.IsValid() );

			Vector4& pos = m_entries[ entry ];

			CellRef child = FindCell( pos );
			AddToCell( child, entry );
		}
	}

	void Octree::FreeCell( Octree::CellRef& cell )
	{
		if( !cell.IsValid() )
			return;

		m_free.Push( cell.Index() );
		m_cells.Zero( cell.Index() );

		cell.Invalidate();
	}

	void Octree::MergeChildren( Octree::CellRef& cell )
	{
		for( CellRef& child : cell->Children )
		{
			// move children's data into the parent
			cell->Data.AddAll( child->Data );

			child->Data.Clear();

			// free the cell
			FreeCell( child );
		}

		ASSERT( !cell->HasChildren() );
	}

	//
	// Remove the given entry.
	//
	void Octree::Remove( const Octree::Entry& entry )
	{
		// find the cell
		CellRef parent;
		const CellRef& cell = FindCell( entry, &parent );

		cell->Remove( entry );

		m_entries.Remove( entry );

		if( !parent.IsValid() )
			return;

		// check siblings to see if we should merge them into the parent
		uint child_count = 0;
		for( const CellRef& child : parent->Children )
		{
			// if siblings have children, then we just don't merge
			if( child->HasChildren() )
				return;

			child_count += child->Data.Size();
		}

		// give a 33% buffer so we don't end up merging too often
		if( child_count < MAX_CELL_ENTRIES * MERGE_THRESHOLD )
		{
			MergeChildren( parent );

			ASSERT( parent->Data.Size() == child_count );
		}
	}

	//
	// Get the position for the given entry.
	//
	const Vector4& Octree::GetPosition( const Octree::Entry& entry ) const
	{
		ASSERT( entry.ID != -1 );

		return m_entries[ entry ];
	}


	//
	// Get the <count> nearest entries to the given entry.
	// 
	void Octree::GetKNearest( const Octree::Entry& entry, uint count, Vector<Octree::Entry>& output )
	{
		ASSERT( entry.ID != -1 );

		output.Clear();

		Vector4 position( m_entries[ entry ] );

		CellRef parent;
		const CellRef& cell = FindCell( entry, &parent );

		// there are potentially 27 cells to consider
		Array<int,32> to_consider;
		to_consider.Push( cell.Index() );

		for( int i = 1; i < 8; ++i )
		{
			Vector4 less( position );
			Vector4 more( position );

			if( i & 4 )
			{
				less.X -= cell->Size;
				more.X += cell->Size;
			}

			if( i & 2 )
			{
				less.Y -= cell->Size;
				more.Y += cell->Size;
			}

			if( i & 1 )
			{
				less.Z -= cell->Size;
				more.Z += cell->Size;
			}

			// clamp inside valid range
			less.X = std::max( ORIGIN.X, less.X );
			less.Y = std::max( ORIGIN.X, less.X );
			less.Z = std::max( ORIGIN.X, less.X );

			more.X = std::min( more.X, m_root->Size - EPSILON );
			more.Y = std::min( more.Y, m_root->Size - EPSILON );
			more.Z = std::min( more.Z, m_root->Size - EPSILON );

			const CellRef& less_ref = FindCell( less );
			if( !to_consider.Contains( less_ref.Index() ) )
			{
				to_consider.Push( less_ref.Index() );
			}

			const CellRef& more_ref = FindCell( more );
			if( !to_consider.Contains( more_ref.Index() ) )
			{
				to_consider.Push( more_ref.Index() );
			}
		}

		for( int cell_index : to_consider )
		{
			Cell& cell = GetCell( cell_index );

			for( const Entry& entry : cell.Data )
			{
				ASSERT( entry.ID != -1 );
				output.Push( entry );
			}
		}

		// sort by distance from the point
		std::sort( output.begin(), output.end(), 
			[&]( const Entry& a, const Entry& b )
		{
			return position.distanceSq( m_entries[ a ] ) < position.distanceSq( m_entries[ b ] );
		} );

		// drop the last elements
		output.Reserve( count );
	}

	//
	// Get all the entries within <range> of the given entry.
	//
	void Octree::GetWithinRange( const Octree::Entry& entry, float range, Vector<Octree::Entry>& output )
	{
		const Vector4& position = m_entries[ entry ];
		float rangeSq = range * range;

		BoundingBox box;
		box.Include( position + range );
		box.Include( position - range );

		GetWithinBounds( box, output );

		// our bounds check was a bit greedy, filter out the entries in the corners
		Vector<Octree::Entry> to_remove;
		for( const Octree::Entry& entry : output )
		{
			if( position.distanceSq( m_entries[ entry ] ) > rangeSq )
				to_remove.Add( entry );
		}

		output.RemoveAll( to_remove );
	}

	//
	// Get all the entries within the given bounding box.
	//
	void Octree::GetWithinBounds( const BoundingBox& bounds, Vector<Octree::Entry>& output )
	{
		Vector<int> leaf_cells;
		Vector<int> to_process;
		to_process.Push( m_root.Index() );

		// breadth-first search of all cells that intersect with the bounds
		while( to_process.Size() > 0 )
		{
			int current_index = to_process.Pop();

			Cell& current = GetCell( current_index );

			if( bounds.Intersects( current.GetBounds() ) )
			{
				if( current.HasChildren() )
				{
					for( const CellRef& child_ref : current.Children )
					{
						to_process.Push( child_ref.Index() );
					}
				}
				else
				{
					leaf_cells.Push( current_index );
				}
			}
		}

		// finer-grained filter to remove those that aren't inside the actual bounds
		for( int index : leaf_cells )
		{
			Cell& cell = GetCell( index );

			for( Entry& entry : cell.Data )
			{
				if( bounds.Contains( m_entries[ entry ] ) )
					output.Add( entry );
			}
		}
	}

	Octree::Entry Octree::NextEntry()
	{
		return Entry( m_lastID++ );
	}

	Octree::CellRef Octree::FindCell( const Vector4& position, Octree::CellRef* parent ) const
	{
		ASSERT( m_root->Contains( position ), "Position is not in the tree!" );

		CellRef current( m_root );

		while( current->HasChildren() )
		{
			if( parent != nullptr )
				*parent = current;

			current = current->GetCellContaining( position );
		}

		ASSERT( current->Contains( position ) );

		return current;
	}

	Octree::CellRef Octree::FindCell( const Octree::Entry& entry, Octree::CellRef* parent ) const
	{
		return FindCell( m_entries[ entry ], parent );
	}

	uint Octree::Count() const 
	{
		return m_entries.Size();
	}

	Octree::Cell& Octree::GetCell( int index ) const
	{
		ASSERT( index >= 0 );

		return m_cells[ index ];
	}

	void Octree::Validate() const
	{
		for( auto entry : m_entries )
		{
			bool bPlaced = false;

			for( const Cell& cell : m_cells )
			{
				ASSERT( cell.Data.Size() == 0 || !cell.HasChildren() ); // cells can't have both children and data

				int index = cell.Data.Find( entry.Key );
				if( index >= 0 )
				{
					ASSERT( !bPlaced ); // check that the entry is only in one cell
					ASSERT( cell.Contains( entry.Value ) ); // check that the entry actually belongs in the cell

					bPlaced = true;
				}
			}
		}
	}
}
