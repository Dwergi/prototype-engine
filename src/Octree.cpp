//
// Octree.cpp - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"
#include "Octree.h"

#define ORIGIN Vector4( 0, 0, 0 )

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

Octree::Entry::Entry( int id )
	: ID( id )
{

}

Octree::Entry::Entry()
	: ID( -1 )
{

}

Octree::Entry::Entry( const Octree::Entry& other )
	: ID( other.ID )
{

}

bool Octree::Entry::operator==( const Octree::Entry& other ) const
{
	return ID == other.ID;
}

//===================================================================================

Octree::CellInternal::CellInternal( const Vector4& pos, float size )
{
	Position = pos;
	Size = size;
}

Octree::CellInternal::CellInternal( const CellInternal& other )
	: Data( other.Data )
{
	Position = other.Position;
	Size = other.Size;

	for( int i = 0; i < 8; ++i )
		Children[ i ] = other.Children[ i ];
}

Octree::CellInternal::~CellInternal()
{
	for( int i = 0; i < 8; ++i )
		Children[ i ].Invalidate();
}

Octree::CellInternal& Octree::CellInternal::operator=( const CellInternal& other )
{
	Data = other.Data;
	Position = other.Position;
	Size = other.Size;

	for( int i = 0; i < 8; ++i )
		Children[ i ] = other.Children[ i ];

	return *this;
}

void Octree::CellInternal::Add( const Octree::Entry& entry )
{
	assert( !HasChildren() );
	assert( Data.size() < MAX_CELL_ENTRIES );

	Data.push_back( entry );
}

void Octree::CellInternal::Remove( const Octree::Entry& entry )
{
	auto it = std::find( Data.begin(), Data.end(), entry );

	if( it != Data.end() )
		Data.erase( it );
}

bool Octree::CellInternal::Contains( const Vector4& pos ) const
{
	return pos >= Position && pos < Position + Size;
}

Octree::Cell Octree::CellInternal::GetCellContaining( const Vector4& pos ) const
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

bool Octree::CellInternal::HasChildren() const
{
	// can't have a single child valid without all children being valid
	return Children[ 0 ].IsValid();
}

BoundingBox Octree::CellInternal::GetBounds() const
{
	return BoundingBox( Position, Position + Size );
}

//===================================================================================

Octree::Octree()
{
	m_lastID = 0;
}

Octree::~Octree()
{
	m_cells.clear();
}

// 
// Add an entry at the given position. 
// Returns a entry used to reference the given object.
//
Octree::Entry Octree::Add( const Vector4& position )
{
	assert( position >= ORIGIN );

	// no root created
	if( !m_root.IsValid() )
	{
		assert( m_cells.empty() );

		m_root = CreateCell( ORIGIN, DEFAULT_SIZE );
	}

	// root is mis-sized
	while( !m_root->Contains( position ) )
	{
		if( m_entries.empty() )
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
	m_entries.insert( std::make_pair( id, position ) );

	Cell cell = FindCell( position );

	AddToCell( cell, id );

	return id;
}

//
// Once we've found the current lowest level cell, we call this which entrys all the potential splits that may result from the add.
// 
void Octree::AddToCell( Octree::Cell cell, const Octree::Entry& entry )
{
	assert( cell->Contains( m_entries[ entry ] ) );

	cell->Add( entry );

	// check if we need to split the cell
	if( cell->Data.size() >= MAX_CELL_ENTRIES )
	{
		SplitCell( cell );
	}
}

//
// Create a new cell at the given position with the given size.
// 
Octree::Cell Octree::CreateCell( const Vector4& pos, float size )
{
	if( m_free.empty() )
	{
		int index = m_cells.size();

		m_cells.emplace_back( CellInternal( pos, size ) );

		return Cell( index, &m_cells );
	}
	else
	{
		Cell cell = m_free.front();
		m_free.pop();

		cell->Position = pos;
		cell->Size = size;

		return cell;
	}
}

void Octree::ExpandRoot()
{
	// copy the old root to the end
	Cell oldRoot = CreateCell( m_root->Position, m_root->Size );
	*oldRoot = *m_root;
	oldRoot->Data.clear();

	// expand the new root
	m_root->Size += m_root->Size;

	// create new children
	CreateChildren( m_root );

	// old root is the first child of the expanded root
	FreeCell( m_root->Children[ 0 ] );
	m_root->Children[ 0 ] = oldRoot;

	// re-add all the entries
	while( !m_root->Data.empty() )
	{
		Entry entry = m_root->Data.front();
		m_root->Data.erase( m_root->Data.begin() );

		Cell cell = FindCell( m_entries[ entry ] );
		AddToCell( cell, entry );
	}
}

void Octree::CreateChildren( Octree::Cell cell )
{
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
void Octree::SplitCell( Octree::Cell cell )
{
	CreateChildren( cell );

	// assign existing entries into new cells
	while( !cell->Data.empty() )
	{
		Entry entry = cell->Data.front();
		cell->Data.erase( cell->Data.begin() );

		Cell child = FindCell( m_entries[ entry ] );
		AddToCell( child, entry );
	}
}

void Octree::FreeCell( Cell cell )
{
	m_free.push( cell );
}

//
// Remove the given entry.
//
void Octree::Remove( const Octree::Entry& entry )
{
	Cell cell = FindCell( m_entries[ entry ] );
	cell->Remove( entry );
}

//
// Get the position for the given entry.
//
const Vector4& Octree::GetPosition( const Octree::Entry& entry ) const
{
	return m_entries.at( entry );
}

//
// Update the position of the given entry.
// 
void Octree::Move( const Octree::Entry& entry, const Vector4& newPos )
{
	Remove( entry );

	m_entries[ entry ] = newPos;

	Cell newCell = FindCell( newPos );
	AddToCell( newCell, entry );
}

//
// Get the <count> nearest entries to the given entry.
// 
void Octree::GetKNearest( const Octree::Entry& entry, int count, std::vector<Octree::Entry>& output )
{
	assert( count > 0 );
	assert( entry.ID != -1 );

	Cell parent;

	Vector4 position = m_entries[ entry ];
	
	Cell current = m_root;
	while( current->HasChildren() )
	{
		parent = current;
		current = current->GetCellContaining( position );
	}

	// there are potentially 27 cells to consider
	std::vector<Cell> to_consider;
	to_consider.reserve( 27 );
	to_consider.push_back( current );

	for( int i = 1; i < 8; ++i )
	{
		Vector4 less( position );
		Vector4 more( position );

		if( i & 4 )
		{
			less.X -= current->Size;
			more.X += current->Size;
		}
		
		if( i & 2 )
		{
			less.Y -= current->Size;
			more.Y += current->Size;
		}

		if( i & 1 )
		{
			less.Z -= current->Size;
			more.Z += current->Size;
		}

		// clamp inside valid range
		less.X = std::max( ORIGIN.X, less.X );
		less.Y = std::max( ORIGIN.X, less.X );
		less.Z = std::max( ORIGIN.X, less.X );

		push_back_unique( to_consider, FindCell( less ) );
		push_back_unique( to_consider, FindCell( more ) );
	}
	
	for( const Cell& cell : to_consider )
	{
		for( const Entry& entry : cell->Data )
			output.push_back( entry );
	}

	// sort by distance from the point
	std::sort( output.begin(), output.end(), 
		[&]( const Entry& a, const Entry& b )
		{
			return position.distanceSq( m_entries[ a ] ) < position.distanceSq( m_entries[ b ] );
		} );

	// drop the last elements
	output.resize( count );
}

//
// Get all the entries within <range> of the given entry.
//
void Octree::GetWithinRange( const Octree::Entry& entry, float range, std::vector<Octree::Entry>& output )
{

}

//
// Get all the entries within the given bounding box.
//
void Octree::GetWithinBounds( const BoundingBox& bounds, std::vector<Octree::Entry>& output )
{
	
}

Octree::Entry Octree::NextEntry()
{
	return Entry( m_lastID++ );
}

Octree::Cell Octree::FindCell( const Vector4& position ) const
{
	assert( m_root->Contains( position ) );

	Cell current = m_root;
	while( current->HasChildren() )
	{
		current = current->GetCellContaining( position );
	}

	assert( current->Contains( position ) );

	return current;
}

Octree::Cell Octree::FindCell( const Octree::Entry& entry ) const
{
	return FindCell( m_entries.at( entry ) );
}

void Octree::Validate() const
{
	for( auto pair : m_entries )
	{
		bool bPlaced = false;

		for( const CellInternal& cell : m_cells )
		{
			assert( cell.Data.empty() || !cell.HasChildren() ); // cells can't have both children and data

			auto it = std::find( cell.Data.begin(), cell.Data.end(), Entry( pair.first ) );
			if( it != cell.Data.end() )
			{
				assert( !bPlaced ); // check that the entry is only in one cell
				assert( cell.Contains( pair.second ) ); // check that the entry actually belongs in the cell

				bPlaced = true;
			}
		}
	}
}