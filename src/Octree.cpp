//
// Octree.cpp - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"
#include "Octree.h"

#include <algorithm>

#define ORIGIN Vector4( 0, 0, 0 )

const float EPSILON = 0.0001f;
const float DEFAULT_SIZE = 10.0f;
const int MAX_CELL_ENTRIES = 32;

template< typename TValue, typename TContainer >
void push_back_unique( TContainer& container, const TValue& value )
{
	auto it = std::find( container.begin(), container.end(), value );
	if( it == container.end() )
		container.insert( container.end(), value );
}

//===================================================================================

dd::Octree::Entry::Entry( int id )
	: ID( id )
{

}

dd::Octree::Entry::Entry()
	: ID( -1 )
{

}

dd::Octree::Entry::Entry( const dd::Octree::Entry& other )
	: ID( other.ID )
{

}

bool dd::Octree::Entry::operator==( const dd::Octree::Entry& other ) const
{
	return ID == other.ID;
}

bool dd::Octree::Entry::IsValid() const
{
	return ID != -1;
}

//===================================================================================

dd::Octree::CellInternal::CellInternal( const dd::Vector4& pos, float size )
{
	Position = pos;
	Size = size;
}

dd::Octree::CellInternal::CellInternal( const CellInternal& other )
	: Data( other.Data )
{
	Position = other.Position;
	Size = other.Size;

	for( int i = 0; i < 8; ++i )
		Children[ i ] = other.Children[ i ];
}

dd::Octree::CellInternal::~CellInternal()
{
	for( int i = 0; i < 8; ++i )
		Children[ i ].Invalidate();
}

dd::Octree::CellInternal& dd::Octree::CellInternal::operator=( const CellInternal& other )
{
	Data = other.Data;
	Position = other.Position;
	Size = other.Size;

	for( int i = 0; i < 8; ++i )
		Children[ i ] = other.Children[ i ];

	return *this;
}

void dd::Octree::CellInternal::Add( const dd::Octree::Entry& entry )
{
	ASSERT( !HasChildren() );
	ASSERT( Data.Size() < MAX_CELL_ENTRIES );

	Data.Push( entry );
}

void dd::Octree::CellInternal::Remove( const dd::Octree::Entry& entry )
{
	Data.Remove( entry );
}

bool dd::Octree::CellInternal::Contains( const dd::Vector4& pos ) const
{
	return pos >= Position && pos < Position + Size;
}

dd::Octree::Cell dd::Octree::CellInternal::GetCellContaining( const dd::Vector4& pos ) const
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

bool dd::Octree::CellInternal::HasChildren() const
{
	// can't have a single child valid without all children being valid
	return Children[ 0 ].IsValid();
}

dd::BoundingBox dd::Octree::CellInternal::GetBounds() const
{
	return BoundingBox( Position, Position + Size );
}

//===================================================================================

dd::Octree::Octree()
{
	m_lastID = 0;
}

dd::Octree::~Octree()
{
	m_cells.Clear();
}

// 
// Add an entry at the given position. 
// Returns a entry used to reference the given object.
//
dd::Octree::Entry dd::Octree::Add( const dd::Vector4& position )
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
			m_root->Size += m_root->Size;
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

	Cell cell = FindCell( position );
	AddToCell( cell, id );

	return id;
}

//
// Once we've found the current lowest level cell, we call this which performs all the potential splits that may result from the add.
// 
void dd::Octree::AddToCell( dd::Octree::Cell cell, const dd::Octree::Entry& entry )
{
	ASSERT( cell->Contains( m_entries[ entry ] ) );

	cell->Add( entry );

	// check if we need to split the cell
	if( cell->Data.Size() >= MAX_CELL_ENTRIES )
	{
		SplitCell( cell );
	}
}

//
// Create a new cell at the given position with the given size.
// 
dd::Octree::Cell dd::Octree::CreateCell( const dd::Vector4& pos, float size )
{
	if( m_free.Size() == 0 )
	{
		size_t index = m_cells.Size();

		m_cells.Allocate( CellInternal( pos, size ) );

		return Cell( (int) index, m_cells );
	}
	else
	{
		Cell cell = m_free[ m_free.Size() - 1 ];
		m_free.Pop();

		cell->Position = pos;
		cell->Size = size;

		return cell;
	}
}

//
// Double the size of the root by adding a node above it.
//
void dd::Octree::ExpandRoot()
{
	// copy the old root to the end
	Cell oldRoot = CreateCell( m_root->Position, m_root->Size );
	*oldRoot = *m_root;
	oldRoot->Data.Clear();

	// expand the new root
	m_root->Size += m_root->Size;

	// create new children
	CreateChildren( m_root );

	// old root is the first child of the expanded root
	FreeCell( m_root->Children[ 0 ] );
	m_root->Children[ 0 ] = oldRoot;

	// re-add all the entries
	while( !m_root->Data.Size() == 0 )
	{
		Entry entry = m_root->Data[ 0 ];
		m_root->Data.Remove( 0 );

		Cell cell = FindCell( m_entries[ entry ] );
		AddToCell( cell, entry );
	}
}

void dd::Octree::CreateChildren( dd::Octree::Cell cell )
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
void dd::Octree::SplitCell( dd::Octree::Cell cell )
{
	ASSERT( cell.IsValid() );

	CreateChildren( cell );

	// assign existing entries into new cells
	while( !cell->Data.Size() == 0 )
	{
		Entry entry = cell->Data[ 0 ];
		cell->Data.Remove( 0 );

		Cell child = FindCell( m_entries[ entry ] );
		AddToCell( child, entry );
	}
}

void dd::Octree::FreeCell( Cell cell )
{
	m_free.Push( cell );
}

//
// Remove the given entry.
//
void dd::Octree::Remove( const dd::Octree::Entry& entry )
{
	// find the cell
	Cell parent;
	Cell cell = FindCell( entry, &parent );

	cell->Remove( entry );

	m_entries.Remove( entry );
	
	// check siblings to see if we should merge them into their parent
	if( parent.IsValid() )
	{
		uint child_count = 0;
		for( Cell& child : parent->Children )
		{
			child_count += child->Data.Size();
		}

		// give a 33% buffer so we don't end up merging too often
		if( child_count < MAX_CELL_ENTRIES * 0.66f )
		{
			for( Cell& child : parent->Children )
			{
				// move children's data into the parent
				while( !child->Data.Size() == 0 )
				{
					parent->Data.Push( child->Data[ 0 ] );
					child->Data.Pop();
				}

				// free the cell
				FreeCell( child );
				
				child.Invalidate();
			}

			ASSERT( !parent->HasChildren() );
			ASSERT( parent->Data.Size() == child_count );
		}
	}
}

//
// Get the position for the given entry.
//
const dd::Vector4& dd::Octree::GetPosition( const dd::Octree::Entry& entry ) const
{
	ASSERT( entry.ID != -1 );

	return m_entries[ entry ];
}


//
// Get the <count> nearest entries to the given entry.
// 
void dd::Octree::GetKNearest( const dd::Octree::Entry& entry, uint count, dd::Vector<dd::Octree::Entry>& output )
{
	ASSERT( entry.ID != -1 );

	output.Clear();

	Vector4 position( m_entries[ entry ] );

	Cell parent;
	Cell cell = FindCell( entry, &parent );

	// there are potentially 27 cells to consider
	std::vector<Cell> to_consider;
	to_consider.reserve( 27 );
	to_consider.push_back( cell );

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

		push_back_unique( to_consider, FindCell( less ) );
		push_back_unique( to_consider, FindCell( more ) );
	}
	
	for( const Cell& cell : to_consider )
	{
		for( const Entry& entry : cell->Data )
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
	output.Resize( count );
}

//
// Get all the entries within <range> of the given entry.
//
void dd::Octree::GetWithinRange( const dd::Octree::Entry& entry, float range, dd::Vector<dd::Octree::Entry>& output )
{
	const Vector4& position = m_entries[ entry ];
	float rangeSq = range * range;

	BoundingBox box;
	box.Include( position + range );
	box.Include( position - range );

	GetWithinBounds( box, output );

	// our bounds check was a bit greedy, filter out the entries in the corners
	dd::Vector<Octree::Entry> to_remove;
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
void dd::Octree::GetWithinBounds( const dd::BoundingBox& bounds, dd::Vector<dd::Octree::Entry>& output )
{
	std::vector<Cell> leaf_cells;
	std::vector<Cell> to_process;
	to_process.push_back( m_root );

	// breadth-first search of all cells that intersect with the bounds
	while( !to_process.empty() )
	{
		Cell current = to_process.back();
		to_process.pop_back();

		if( bounds.Intersects( current->GetBounds() ) )
		{
			if( current->HasChildren() )
			{
				for( Cell& child : current->Children )
				{
					to_process.push_back( child );
				}
			}
			else
			{
				leaf_cells.push_back( current );
			}
		}
	}

	// finer-grained filter to remove those that aren't inside the actual bounds
	for( Cell& cell : leaf_cells )
	{
		for( Entry& entry : cell->Data )
		{
			if( bounds.Contains( m_entries[ entry ] ) )
				output.Add( entry );
		}
	}
}

dd::Octree::Entry dd::Octree::NextEntry()
{
	return Entry( m_lastID++ );
}

dd::Octree::Cell dd::Octree::FindCell( const dd::Vector4& position, Cell* parent ) const
{
	ASSERT( m_root->Contains( position ), "Position is not in the tree!" );

	Cell current = m_root;
	while( current->HasChildren() )
	{
		if( parent != nullptr )
			*parent = current;

		current = current->GetCellContaining( position );
	}

	ASSERT( current->Contains( position ) );

	return current;
}

dd::Octree::Cell dd::Octree::FindCell( const Octree::Entry& entry, Cell* parent ) const
{
	return FindCell( m_entries[ entry ], parent );
}

int dd::Octree::Count() const 
{
	return (int) m_entries.Size();
}

void dd::Octree::Validate() const
{
	for( auto entry : m_entries )
	{
		bool bPlaced = false;

		for( const CellInternal& cell : m_cells )
		{
			ASSERT( cell.Data.Size() == 0 || !cell.HasChildren() ); // cells can't have both children and data

			int index = cell.Data.IndexOf( Entry( entry.Key ) );
			if( index >= 0 )
			{
				ASSERT( !bPlaced ); // check that the entry is only in one cell
				ASSERT( cell.Contains( entry.Value ) ); // check that the entry actually belongs in the cell

				bPlaced = true;
			}
		}
	}
}