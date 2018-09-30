//
// Triangulator.cpp - A mesh triangulator utility.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#include "PCH.h"
#include "Triangulator.h"

namespace dd
{
	Triangulator::Triangulator( const dd::Buffer<glm::vec3>& pos ) :
		m_size( pos.Size() / 3 )
	{
		m_positions = pos.Get();
	}

	Triangulator::Triangulator( const dd::Buffer<glm::vec3>& pos, const dd::ConstBuffer<uint>& idx ) :
		m_size( idx.Size() / 3 )
	{
		m_positions = pos.Get();
		m_indices = idx.GetConst();
	}

	Triangulator::Triangulator( std::vector<glm::vec3>& pos ) :
		m_size( pos.size() / 3 )
	{
		m_positions = pos.data();
	}

	Triangulator::Triangulator( std::vector<glm::vec3>& pos, const std::vector<uint>& idx ) : 
		m_size( idx.size() / 3 )
	{
		m_positions = pos.data();
		m_indices = idx.data();
	}

	Triangle Triangulator::operator[]( size_t i ) const
	{
		DD_ASSERT( i < Size() );

		if( m_indices == nullptr )
		{
			Triangle tri( m_positions[ i * 3 + 0 ], m_positions[ i * 3 + 1 ], m_positions[ i * 3 + 2 ] );
			return tri;
		}

		uint i0 = m_indices[ i * 3 + 0 ];
		uint i1 = m_indices[ i * 3 + 1 ];
		uint i2 = m_indices[ i * 3 + 2 ];

		Triangle tri( m_positions[ i0 ], m_positions[ i1 ], m_positions[ i2 ], i0, i1, i2 );
		return tri;
	}

	ConstTriangulator::ConstTriangulator( const dd::ConstBuffer<glm::vec3>& pos ) :
		m_size( pos.Size() / 3 )
	{
		m_positions = pos.GetConst();
	}

	ConstTriangulator::ConstTriangulator( const dd::ConstBuffer<glm::vec3>& pos, const dd::ConstBuffer<uint>& idx ) :
		m_size( idx.Size() / 3 )
	{
		m_positions = pos.GetConst();
		m_indices = idx.GetConst();
	}

	ConstTriangulator::ConstTriangulator( const std::vector<glm::vec3>& pos ) : 
		m_size( pos.size() / 3 )
	{
		m_positions = pos.data();
	}

	ConstTriangulator::ConstTriangulator( const std::vector<glm::vec3>& pos, const std::vector<uint>& idx ) :
		m_size( idx.size() / 3 )
	{
		m_positions = pos.data();
		m_indices = idx.data();
	}

	ConstTriangle ConstTriangulator::operator[]( size_t i ) const
	{
		DD_ASSERT( i < Size() );

		if( m_indices == nullptr )
		{
			ConstTriangle tri( m_positions[ i * 3 + 0 ], m_positions[ i * 3 + 1 ], m_positions[ i * 3 + 2 ] );
			return tri;
		}

		uint i0 = m_indices[ i * 3 + 0 ];
		uint i1 = m_indices[ i * 3 + 1 ];
		uint i2 = m_indices[ i * 3 + 2 ];

		ConstTriangle tri( m_positions[ i0 ], m_positions[ i1 ], m_positions[ i2 ], i0, i1, i2 );
		return tri;
	}
}