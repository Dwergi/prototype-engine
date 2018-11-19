#include "PCH.h"
#include "TerrainTile.h"

#include "glm/gtc/noise.hpp"

namespace dd
{
	std::vector<dd::Buffer<uint>> TerrainTile::s_indices;

	static size_t GetIndexCount( uint lod )
	{
		size_t actual_verts = TerrainTile::MaxVertices / (1 << lod);
		size_t mesh_indices = actual_verts * actual_verts * 6;
		size_t flap_indices = actual_verts * 6 * 4;
		return mesh_indices + flap_indices;
	}

	void TerrainTile::Initialize()
	{
		for( uint lod = 0; lod <= MaxLODs; ++lod )
		{
			size_t index_count = GetIndexCount( lod );
			s_indices.push_back( dd::Buffer<uint>( new uint[ index_count ], index_count ) );

			GenerateIndices( lod );
		}

		GeneratePositions( glm::vec2( 0, 0 ), 8 );

		m_positionsBuffer.Set( m_positions, TotalVertexCount );
	}

	float TerrainTile::GetHeight( float x, float y ) const
	{
		float height = 0;
		float wavelength = m_terrainParams.Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < m_terrainParams.Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec3 coord( x * multiplier, y * multiplier, m_terrainParams.Seed );

			float noise = glm::simplex( coord );
			height += noise * m_terrainParams.Amplitudes[ i ];

			total_amplitude += m_terrainParams.Amplitudes[ i ];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainTile::GeneratePositions( glm::vec2 tile_pos, uint lod )
	{
		DD_ASSERT( lod <= MaxLODs );

		const uint stride = 1 << lod;
		constexpr int actual_vertices = MaxVertices + 1;

		constexpr float max_height = std::numeric_limits<float>::min();
		constexpr float min_height = std::numeric_limits<float>::max();

		for( int z = 0; z < actual_vertices; z += stride )
		{
			for( int x = 0; x < actual_vertices; x += stride )
			{
				const int current = z * actual_vertices + x;

				const float x_coord = tile_pos.x + x * VertexDistance;
				const float z_coord = tile_pos.y + z * VertexDistance;

				m_positions[ current ] = glm::vec3( x * VertexDistance, GetHeight( x_coord, z_coord ), z * VertexDistance );
			}
		}

		m_bounds.Min = glm::vec3( 0, min_height, 0 );
		m_bounds.Max = glm::vec3( MaxVertices * VertexDistance, max_height, MaxVertices * VertexDistance );

		const float chunk_size = MaxVertices * VertexDistance;
		int flap_vertex_start = MeshVertexCount;
		for( int x = 0; x < actual_vertices; x += stride )
		{
			m_positions[ flap_vertex_start + x ] = glm::vec3( x * VertexDistance, 0, 0 );
		}

		flap_vertex_start += actual_vertices;
		for( int z = 0; z < actual_vertices; z += stride )
		{
			m_positions[ flap_vertex_start + z ] = glm::vec3( chunk_size, 0, z * VertexDistance );
		}

		flap_vertex_start += actual_vertices;
		for( int x = 0; x < actual_vertices; x += stride )
		{
			m_positions[ flap_vertex_start + x ] = glm::vec3( x * VertexDistance, 0, chunk_size );
		}

		flap_vertex_start += actual_vertices;
		for( int z = 0; z < actual_vertices; z += stride )
		{
			m_positions[ flap_vertex_start + z ] = glm::vec3( 0, 0, z * VertexDistance );
		}
	}

	void TerrainTile::GenerateIndices( uint lod )
	{
		const uint stride = 1 << lod;

		dd::Buffer<uint>& indices = s_indices[ lod ];

		uint index = 0;
		const uint actual_vertices = MaxVertices + 1;

		for( uint z = 0; z < actual_vertices - 1; z += stride )
		{
			for( uint x = 0; x < actual_vertices; x += stride )
			{
				const uint current = z * actual_vertices + x;
				const uint next_row = (z + stride) * actual_vertices + x;

				DD_ASSERT( next_row < actual_vertices * actual_vertices );

				if( x != 0 )
				{
					indices[ index ] = current;
					indices[ index + 1 ] = next_row - stride;
					indices[ index + 2 ] = next_row;

					index += 3;
				}

				if( x != MaxVertices )
				{
					indices[ index ] = current;
					indices[ index + 1 ] = next_row;
					indices[ index + 2 ] = current + stride;

					index += 3;
				}
			}
		}

		int flap_vertex_start = MeshVertexCount;

		// (top, x = varying, z = 0)
		for( uint x = 0; x < MaxVertices; x += stride )
		{
			indices[ index + 0 ] = x;
			indices[ index + 1 ] = flap_vertex_start + x + stride;
			indices[ index + 2 ] = flap_vertex_start + x;

			index += 3;

			indices[ index + 0 ] = x;
			indices[ index + 1 ] = x + stride;
			indices[ index + 2 ] = flap_vertex_start + x + stride;

			index += 3;
		}

		for( size_t i = flap_vertex_start; i < indices.Size(); ++i )
		{
			DD_ASSERT( indices[ i ] < TotalVertexCount );
		}

		flap_vertex_start += actual_vertices;

		// (right, x = chunk size, z = varying)
		for( uint z = 0; z < MaxVertices; z += stride )
		{
			indices[ index + 0 ] = (z + 2 * stride) * actual_vertices - stride;
			indices[ index + 1 ] = flap_vertex_start + z + stride;
			indices[ index + 2 ] = flap_vertex_start + z;

			index += 3;

			indices[ index + 0 ] = flap_vertex_start + z;
			indices[ index + 1 ] = (z + stride) * actual_vertices - stride;
			indices[ index + 2 ] = (z + 2 * stride) * actual_vertices - stride;

			index += 3;
		}

		for( size_t i = flap_vertex_start; i < indices.Size(); ++i )
		{
			DD_ASSERT( indices[ i ] < TotalVertexCount );
		}

		flap_vertex_start += actual_vertices;

		// (bottom, x = varying, z = chunk size)
		for( uint x = 0; x < MaxVertices; x += stride )
		{
			const uint last_row = MeshVertexCount - actual_vertices;

			indices[ index + 0 ] = last_row + x + stride;
			indices[ index + 1 ] = flap_vertex_start + x;
			indices[ index + 2 ] = flap_vertex_start + x + stride;

			index += 3;

			indices[ index + 0 ] = last_row + x;
			indices[ index + 1 ] = flap_vertex_start + x;
			indices[ index + 2 ] = last_row + x + stride;

			index += 3;
		}

		for( size_t i = flap_vertex_start; i < indices.Size(); ++i )
		{
			DD_ASSERT( indices[ i ] < TotalVertexCount );
		}

		flap_vertex_start += actual_vertices;

		// (left, x = 0, z = varying)
		for( uint z = 0; z < MaxVertices; z += stride )
		{
			indices[ index + 0 ] = (z + stride) * actual_vertices;
			indices[ index + 1 ] = flap_vertex_start + z;
			indices[ index + 2 ] = flap_vertex_start + z + stride;

			index += 3;

			indices[ index + 0 ] = z * actual_vertices;
			indices[ index + 1 ] = flap_vertex_start + z;
			indices[ index + 2 ] = (z + stride) * actual_vertices;

			index += 3;
		}

		for( size_t i = flap_vertex_start; i < indices.Size(); ++i )
		{
			DD_ASSERT( indices[ i ] < TotalVertexCount );
		}
	}
}