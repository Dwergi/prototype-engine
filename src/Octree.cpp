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

bool Octree::Entry::IsValid() const
{
	return ID != -1;
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
	ASSERT( !HasChildren() );
	ASSERT( Data.size() < MAX_CELL_ENTRIES );

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
	ASSERT( position >= ORIGIN );

	// no root created
	if( !m_root.IsValid() )
	{
		ASSERT( m_cells.empty() );

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

	ASSERT( id.IsValid() );

	m_entries.insert( std::make_pair( id, position ) );

	ASSERT( m_entries.find( Entry( -1 ) ) == m_entries.end() );

	Cell cell = FindCell( position );
	AddToCell( cell, id );

	return id;
}

//
// Once we've found the current lowest level cell, we call this which entrys all the potential splits that may result from the add.
// 
void Octree::AddToCell( Octree::Cell cell, const Octree::Entry& entry )
{
	ASSERT( cell->Contains( m_entries.at( entry ) ) );

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

		Cell cell = FindCell( m_entries.at( entry ) );
		AddToCell( cell, entry );
	}
}

void Octree::CreateChildren( Octree::Cell cell )
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
void Octree::SplitCell( Octree::Cell cell )
{
	ASSERT( cell.IsValid() );

	CreateChildren( cell );

	// assign existing entries into new cells
	while( !cell->Data.empty() )
	{
		Entry entry = cell->Data.front();
		cell->Data.erase( cell->Data.begin() );

		Cell child = FindCell( m_entries.at( entry ) );
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
	// find the cell
	Cell parent;
	Cell cell = FindCell( entry, &parent );

	cell->Remove( entry );

	auto it = m_entries.find( entry );
	if( it != m_entries.end() )
		m_entries.erase( it );

	ASSERT( m_entries.find( Entry( -1 ) ) == m_entries.end() );
	
	// check siblings to see if we should merge them into their parent
	if( parent.IsValid() )
	{
		int child_count = 0;
		for( Cell& child : parent->Children )
		{
			child_count += child->Data.size();
		}

		// give a 33% buffer so we don't end up merging too often
		if( child_count < MAX_CELL_ENTRIES * 0.66f )
		{
			for( Cell& child : parent->Children )
			{
				// move childrens' data into the parent
				while( !child->Data.empty() )
				{
					parent->Data.push_back( child->Data.back() );
					child->Data.pop_back();
				}

				// free the cell
				FreeCell( child );
				
				child.Invalidate();
			}

			ASSERT( !parent->HasChildren() );
			ASSERT( parent->Data.size() == child_count );
		}
	}
}

//
// Get the position for the given entry.
//
const Vector4& Octree::GetPosition( const Octree::Entry& entry ) const
{
	ASSERT( entry.ID != -1 );

	return m_entries.at( entry );
}


//
// Get the <count> nearest entries to the given entry.
// 
void Octree::GetKNearest( const Octree::Entry& entry, int count, std::vector<Octree::Entry>& output )
{
	ASSERT( count > 0 );
	ASSERT( entry.ID != -1 );

	output.clear();

	Vector4 position = m_entries.at( entry );

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

		push_back_unique( to_consider, FindCell( less ) );
		push_back_unique( to_consider, FindCell( more ) );
	}
	
	for( const Cell& cell : to_consider )
	{
		for( const Entry& entry : cell->Data )
		{
			ASSERT( entry.ID != -1 );
			output.push_back( entry );
		}
	}

	// sort by distance from the point
	std::sort( output.begin(), output.end(), 
		[&]( const Entry& a, const Entry& b )
		{
			return position.distanceSq( m_entries.at( a ) ) < position.distanceSq( m_entries.at( b ) );
		} );

	// drop the last elements
	output.resize( count );
}

//
// Get all the entries within <range> of the given entry.
//
void Octree::GetWithinRange( const Octree::Entry& entry, float range, std::vector<Octree::Entry>& output )
{
	const Vector4& position = m_entries.at( entry );
	float rangeSq = range * range;

	BoundingBox box;
	box.Include( position + range );
	box.Include( position - range );

	GetWithinBounds( box, output );

	// our bounds check was a bit greedy, filter out the entries in the corners
	std::remove_if( output.begin(), output.end(), [&]( const Octree::Entry& entry )
	{
		return position.distanceSq( m_entries.at( entry ) ) > rangeSq;
	} );
}

//
// Get all the entries within the given bounding box.
//
void Octree::GetWithinBounds( const BoundingBox& bounds, std::vector<Octree::Entry>& output )
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
			if( bounds.Contains( m_entries.at( entry ) ) )
				output.push_back( entry );
		}
	}
}

Octree::Entry Octree::NextEntry()
{
	return Entry( m_lastID++ );
}

Octree::Cell Octree::FindCell( const Vector4& position, Cell* parent ) const
{
	ASSERT( m_root->Contains( position ) );

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

Octree::Cell Octree::FindCell( const Octree::Entry& entry, Cell* parent ) const
{
	return FindCell( m_entries.at( entry ), parent );
}

int Octree::Count() const 
{
	return m_entries.size();
}

void Octree::Validate() const
{
	for( auto pair : m_entries )
	{
		bool bPlaced = false;

		for( const CellInternal& cell : m_cells )
		{
			ASSERT( cell.Data.empty() || !cell.HasChildren() ); // cells can't have both children and data

			auto it = std::find( cell.Data.begin(), cell.Data.end(), Entry( pair.first ) );
			if( it != cell.Data.end() )
			{
				ASSERT( !bPlaced ); // check that the entry is only in one cell
				ASSERT( cell.Contains( pair.second ) ); // check that the entry actually belongs in the cell

				bPlaced = true;
			}
		}
	}
}