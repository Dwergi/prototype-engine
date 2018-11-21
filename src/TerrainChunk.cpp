//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PCH.h"
#include "TerrainChunk.h"

#include "ICamera.h"
#include "GLError.h"
#include "JobSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TerrainChunkKey.h"
#include "TerrainParameters.h"
#include "Uniforms.h"

#include "glm/gtc/noise.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	std::vector<uint> TerrainChunk::s_indices[ MaxLODs ];
	ConstBuffer<uint> TerrainChunk::s_indexBuffers[ MaxLODs ];

	ddr::ShaderHandle TerrainChunk::s_shader;
	ddr::MaterialHandle TerrainChunk::s_material;

	TerrainChunk::TerrainChunk( const TerrainParameters& params, const TerrainChunkKey& key ) :
		m_terrainParams( params ),
		m_key( key )
	{
		m_verticesBuffer.Set( m_vertices, TotalVertexCount );
	}

	TerrainChunk::~TerrainChunk()
	{
		ddr::Mesh::Destroy( m_mesh );

		m_verticesBuffer.Release();
	}

	void TerrainChunk::InitializeShared()
	{
		for( uint lod = 0; lod < MaxLODs; ++lod )
		{
			const uint stride = 1 << lod;
			const uint lod_vertices = MaxVertices / stride;
			const uint row_width = MaxVertices + 1;
			const uint mesh_index_count = lod_vertices * lod_vertices * 6;
			const uint flap_index_count = lod_vertices * 6 * 4;

			DD_ASSERT( stride <= MaxVertices );

			std::vector<uint>& indices = s_indices[ lod ];
			indices.reserve( mesh_index_count + flap_index_count );

			for( uint z = 0; z < MaxVertices; z += stride )
			{
				for( uint x = 0; x < MaxVertices + 1; x += stride )
				{
					const uint current = z * row_width + x;
					const uint next_row = (z + stride) * row_width + x;

					DD_ASSERT( next_row < row_width * row_width );

					if( x != 0 )
					{
						indices.push_back( current );
						indices.push_back( next_row - stride );
						indices.push_back( next_row );
					}

					if( x != MaxVertices )
					{
						indices.push_back( current );
						indices.push_back( next_row );
						indices.push_back( current + stride );
					}
				}
			}

			DD_ASSERT( indices.size() == mesh_index_count );

			int flap_vertex_start = MeshVertexCount;

			// (top, x = varying, z = 0)
			for( uint x = 0; x < MaxVertices; x += stride )
			{
				indices.push_back( x );
				indices.push_back( flap_vertex_start + x + stride );
				indices.push_back( flap_vertex_start + x );

				indices.push_back( x );
				indices.push_back( x + stride );
				indices.push_back( flap_vertex_start + x + stride );
			}

			flap_vertex_start += row_width;

			// (right, x = chunk size, z = varying)
			for( uint z = 0; z < MaxVertices; z += stride )
			{
				indices.push_back( (z + stride) * row_width + MaxVertices );
				indices.push_back( flap_vertex_start + z + stride );
				indices.push_back( flap_vertex_start + z );

				indices.push_back( flap_vertex_start + z );
				indices.push_back( z * row_width + MaxVertices );
				indices.push_back( (z + stride) * row_width + MaxVertices );
			}

			flap_vertex_start += row_width;

			// (bottom, x = varying, z = chunk size)
			for( uint x = 0; x < MaxVertices; x += stride )
			{
				const uint last_row = MeshVertexCount - row_width;

				indices.push_back( last_row + x + stride );
				indices.push_back( flap_vertex_start + x );
				indices.push_back( flap_vertex_start + x + stride );

				indices.push_back( last_row + x );
				indices.push_back( flap_vertex_start + x );
				indices.push_back( last_row + x + stride );
			}

			flap_vertex_start += row_width;

			// (left, x = 0, z = varying)
			for( uint z = 0; z < MaxVertices; z += stride )
			{
				indices.push_back( (z + stride) * row_width );
				indices.push_back( flap_vertex_start + z );
				indices.push_back( flap_vertex_start + z + stride );

				indices.push_back( z * row_width );
				indices.push_back( flap_vertex_start + z );
				indices.push_back( (z + stride) * row_width );
			}

			DD_ASSERT( indices.size() == mesh_index_count + flap_index_count );

			for( uint i : indices )
			{
				DD_ASSERT( i < TotalVertexCount );
			}

			s_indexBuffers[ lod ].Set( indices.data(), (int) indices.size() );
		}
	}

	void TerrainChunk::CreateRenderResources()
	{
		s_shader = ddr::ShaderProgram::Load( "terrain" );
		s_material = ddr::Material::Create( "terrain" );

		ddr::Material* material = ddr::Material::Get( s_material );
		material->SetShader( s_shader );
	}

	void TerrainChunk::Generate()
	{
		DD_PROFILE_SCOPED( TerrainChunk_Generate );

		const int row_width = MaxVertices + 1;
		const float vertex_distance = m_terrainParams.ChunkSize / MaxVertices;

		for( int z = 0; z < row_width; ++z )
		{
			for( int x = 0; x < row_width; ++x )
			{
				const int current = z * row_width + x;

				// height is y
				m_vertices[ current ] = glm::vec3( x * vertex_distance, 0, z * vertex_distance );
			}
		}

		int flap_vertex_start = MeshVertexCount;
		for( int x = 0; x < row_width; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( x * vertex_distance, 0, 0 );
		}

		flap_vertex_start += row_width;
		for( int z = 0; z < row_width; ++z )
		{
			m_vertices[flap_vertex_start + z] = glm::vec3( m_terrainParams.ChunkSize, 0, z * vertex_distance );
		}

		flap_vertex_start += row_width;
		for( int x = 0; x < row_width; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( x * vertex_distance, 0, m_terrainParams.ChunkSize );
		}

		flap_vertex_start += row_width;
		for( int z = 0; z < row_width; ++z )
		{
			m_vertices[flap_vertex_start + z] = glm::vec3( 0, 0, z * vertex_distance );
		}

		m_dataDirty = true;
	}

	void TerrainChunk::Update( JobSystem& job_system )
	{
		if( m_dataDirty )
		{
			job_system.Schedule(
				[this]()
			{
				UpdateVertices( m_noiseOffset ); 

				DD_TODO( "Do I need normals or is geometry shader better?" );
				//UpdateNormals();

				m_dataDirty = false;
				m_renderDirty = true;
			} );
		}
	}

	void TerrainChunk::RenderUpdate( ddr::UniformStorage& uniforms )
	{
		if( m_destroy )
		{
			ddr::Mesh::Destroy( m_mesh );
		}

		if( m_renderDirty )
		{
			ddr::Mesh* mesh;

			if( m_mesh.IsValid() )
			{
				mesh = ddr::Mesh::Get( m_mesh );
			}
			else
			{
				mesh = CreateMesh( m_key );
			}

			mesh->SetBoundBox( m_bounds );
			mesh->SetDirty();

			m_renderDirty = false;
		}
	}

	void TerrainChunk::Destroy()
	{
		m_destroy = true;
	}

	float TerrainChunk::GetNoise( float x, float y )
	{
		float height = 0;
		float wavelength = m_terrainParams.Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < m_terrainParams.Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec3 coord( x * multiplier, y * multiplier, m_terrainParams.Seed );

			float noise = glm::simplex( coord );
			height += noise * m_terrainParams.Amplitudes[i];

			total_amplitude += m_terrainParams.Amplitudes[i];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainChunk::UpdateVertices( glm::vec2 origin )
	{
		if( m_key.LOD >= m_minLod )
		{
			return;
		}

		DD_ASSERT( m_key.LOD < MaxLODs );

		glm::vec2 chunk_pos = origin + glm::vec2( m_key.X, m_key.Y );

		const uint stride = 1 << m_key.LOD;
		DD_ASSERT( stride < MaxVertices );

		const int row_width = MaxVertices + 1;
		const float vertex_distance = m_terrainParams.ChunkSize / MaxVertices;

		float max_height = std::numeric_limits<float>::min();
		float min_height = std::numeric_limits<float>::max();

		for( int z = 0; z < row_width; z += stride )
		{
			for( int x = 0; x < row_width; x += stride )
			{
				const float x_coord = chunk_pos.x + x * vertex_distance;
				const float z_coord = chunk_pos.y + z * vertex_distance;

				float height = GetNoise( x_coord, z_coord );

				// height is y
				float normalized_height = (1 + height) / 2;
				DD_ASSERT( normalized_height >= 0 && normalized_height <= 1 );

				const int current = z * row_width + x;
				DD_ASSERT( current < MeshVertexCount );

				m_vertices[current].y = normalized_height * m_terrainParams.HeightRange;

				max_height = ddm::max( max_height, m_vertices[current].y );
				min_height = ddm::min( min_height, m_vertices[current].y );
			}
		}

		m_position = glm::vec3( chunk_pos.x, 0, chunk_pos.y );

		m_bounds.Min = glm::vec3( 0, min_height, 0 );
		m_bounds.Max = glm::vec3( m_terrainParams.ChunkSize, max_height, m_terrainParams.ChunkSize );

		m_minLod = m_key.LOD;

		int flap_vertex_start = MeshVertexCount;
		for( int i = 0; i < row_width; i += stride )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].z == 0.0f );
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].y == 0.0f );
		}

		flap_vertex_start += row_width;
		for( int i = 0; i < row_width; i += stride )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].x == m_terrainParams.ChunkSize );
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].y == 0.0f );
		}

		flap_vertex_start += row_width;
		for( int i = 0; i < row_width; i += stride )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].z == m_terrainParams.ChunkSize );
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].y == 0.0f );
		}

		flap_vertex_start += row_width;
		for( int i = 0; i < row_width; i += stride )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].x == 0.0f );
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].y == 0.0f );
		}
	}

	void TerrainChunk::SetNoiseOffset( glm::vec2 noise_offset )
	{
		m_noiseOffset = noise_offset;
		m_dataDirty = true;
	}

	ddr::Mesh* TerrainChunk::CreateMesh( const TerrainChunkKey& key )
	{
		DD_PROFILE_SCOPED( TerrainChunk_CreateMesh );

		char name[ 128 ];
		sprintf_s( name, 128, "%.2fx%.2f_%d", key.X, key.Y, key.LOD );

		m_mesh = ddr::Mesh::Create( name );

		ddr::Mesh* mesh = ddr::Mesh::Get( m_mesh );
		mesh->SetMaterial( s_material );
		mesh->SetPositions( m_verticesBuffer );
		mesh->SetIndices( s_indexBuffers[ key.LOD ] );

		return mesh;
	}

	void TerrainChunk::WriteHeightImage( const char* filename ) const
	{
		const int actual_vertices = MaxVertices + 1;
		byte pixels[actual_vertices * actual_vertices];

		for( int y = 0; y < actual_vertices; ++y )
		{
			for( int x = 0; x < actual_vertices; ++x )
			{
				pixels[y * actual_vertices + x] = (byte) ((m_vertices[y * actual_vertices + x].y / m_terrainParams.HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, actual_vertices, actual_vertices, 1, pixels );
	}

	static byte NormalToColour( float f )
	{
		return (byte) (((f + 1.0f) / 2.0f) * 255.f);
	}
}