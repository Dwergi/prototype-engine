//
// Triangulator.cpp - A mesh triangulator utility.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#include "PCH.h"
#include "Triangulator.h"

#include "Mesh.h"

namespace dd
{
	Triangulator::Triangulator( ddr::Mesh& mesh ) 
	{
		dd::Buffer<glm::vec3> positions = mesh.AccessPositions();
		DD_ASSERT( positions.IsValid() );
		
		m_positions = positions.Access();

		dd::ConstBuffer<uint> indices = mesh.GetIndices();
		if( indices.IsValid() )
		{
			m_indices = indices.Get();
			m_size = indices.Size() / 3;
		}
		else
		{
			m_size = positions.Size() / 3;
		}
	}

	Triangulator::Triangulator( const dd::Buffer<glm::vec3>& pos )
	{
		m_positions = pos.Access();
		m_size = pos.Size() / 3;
	}

	Triangulator::Triangulator( const dd::Buffer<glm::vec3>& pos, const dd::ConstBuffer<uint>& idx )
	{
		if( idx.IsValid() )
		{
			m_positions = pos.Access();
			m_indices = idx.Get();
			m_size = idx.Size() / 3;
		}
		else
		{
			m_positions = pos.Access();
			m_size = pos.Size() / 3;
		}
	}

	Triangulator::Triangulator( std::vector<glm::vec3>& pos )
	{
		m_positions = pos.data();
		m_size = pos.size() / 3;
	}

	Triangulator::Triangulator( std::vector<glm::vec3>& pos, const std::vector<uint>& idx )
	{
		if( !idx.empty() )
		{
			m_positions = pos.data();
			m_indices = idx.data();
			m_size = idx.size() / 3;
		}
		else
		{
			m_positions = pos.data();
			m_size = pos.size() / 3;
		}
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

	ConstTriangulator::ConstTriangulator( const ddr::Mesh& mesh )
	{
		dd::ConstBuffer<glm::vec3> positions = mesh.GetPositions();
		DD_ASSERT( positions.IsValid() );

		m_positions = positions.Get();

		dd::ConstBuffer<uint> indices = mesh.GetIndices();
		if( indices.IsValid() )
		{
			m_indices = indices.Get();
			m_size = indices.Size() / 3;
		}
		else
		{
			m_size = mesh.GetPositions().Size() / 3;
		}
	}

	ConstTriangulator::ConstTriangulator( const dd::ConstBuffer<glm::vec3>& pos )
	{
		m_positions = pos.Get();
		m_size = pos.Size() / 3;
	}

	ConstTriangulator::ConstTriangulator( const dd::ConstBuffer<glm::vec3>& pos, const dd::ConstBuffer<uint>& idx )
	{
		if( idx.IsValid() )
		{
			m_positions = pos.Get();
			m_indices = idx.Get();
			m_size = idx.Size() / 3;
		}
		else
		{
			m_positions = pos.Get();
			m_size = pos.Size() / 3;
		}
	}

	ConstTriangulator::ConstTriangulator( const std::vector<glm::vec3>& pos )
	{
		m_positions = pos.data();
		m_size = pos.size() / 3;
	}

	ConstTriangulator::ConstTriangulator( const std::vector<glm::vec3>& pos, const std::vector<uint>& idx )
	{
		if( !idx.empty() )
		{
			m_positions = pos.data();
			m_indices = idx.data();
			m_size = idx.size() / 3;
		}
		else
		{
			m_positions = pos.data();
			m_size = pos.size() / 3;
		}
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