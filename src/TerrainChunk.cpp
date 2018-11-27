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
#include "ShaderPart.h"
#include "Shader.h"
#include "TerrainParameters.h"
#include "Uniforms.h"

#include "glm/gtc/noise.hpp"

#include "stb/stb_image_write.h"

#include "fmt/format.h"

namespace dd
{
	std::vector<uint> TerrainChunk::s_indices[ TerrainParameters::LODs ];
	ConstBuffer<uint> TerrainChunk::s_indexBuffers[ TerrainParameters::LODs ];

	ddr::ShaderHandle TerrainChunk::s_shader;
	ddr::MaterialHandle TerrainChunk::s_material;

	FSMPrototype TerrainChunk::s_fsmPrototype;

	TerrainChunk::TerrainChunk( JobSystem& jobsystem, const TerrainParameters& params, glm::vec2 position ) :
		m_jobsystem( jobsystem ),
		m_terrainParams( params ),
		m_state( s_fsmPrototype ),
		m_position( position )
	{
		m_vertices.resize( TotalVertexCount );
		m_verticesBuffer.Set( m_vertices.data(), TotalVertexCount );
	}

	TerrainChunk::~TerrainChunk()
	{
		ddr::MeshManager::Instance()->Destroy( m_mesh );

		m_verticesBuffer.ReleaseConst();
	}

	void TerrainChunk::InitializeShared()
	{
		DD_ASSERT( s_indices[ 0 ].empty(), "Multiple calls of InitializeShared!" );

		for( uint lod = 0; lod < TerrainParameters::LODs; ++lod )
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

		s_fsmPrototype.AddState( INITIALIZE_PENDING );
		s_fsmPrototype.SetInitialState( INITIALIZE_PENDING );

		s_fsmPrototype.AddState( INITIALIZE_DONE );
		s_fsmPrototype.AddTransition( INITIALIZE_PENDING, INITIALIZE_DONE );

		s_fsmPrototype.AddState( UPDATE_PENDING );
		s_fsmPrototype.AddTransition( INITIALIZE_DONE, UPDATE_PENDING );

		s_fsmPrototype.AddState( UPDATE_DONE );
		s_fsmPrototype.AddTransition( UPDATE_PENDING, UPDATE_DONE );

		s_fsmPrototype.AddState( RENDER_UPDATE_PENDING );
		s_fsmPrototype.AddTransition( UPDATE_DONE, RENDER_UPDATE_PENDING );

		s_fsmPrototype.AddState( RENDER_UPDATE_DONE );
		s_fsmPrototype.AddTransition( RENDER_UPDATE_PENDING, RENDER_UPDATE_DONE );

		s_fsmPrototype.AddState( READY );
		s_fsmPrototype.AddTransition( RENDER_UPDATE_DONE, READY );

		s_fsmPrototype.AddTransition( UPDATE_DONE, UPDATE_PENDING );
		s_fsmPrototype.AddTransition( READY, UPDATE_PENDING );
	}

	void TerrainChunk::CreateRenderResources()
	{
		s_shader = ddr::ShaderManager::Instance()->Load( "terrain" );
		s_material = ddr::MaterialManager::Instance()->Create( "terrain" );

		ddr::Material* material = s_material.Access();
		material->SetShader( s_shader );
	}

	void TerrainChunk::Initialize()
	{
		DD_PROFILE_SCOPED( TerrainChunk_Initialize );
		DD_ASSERT( m_state == INITIALIZE_PENDING );

		const int row_width = MaxVertices + 1;
		const float vertex_distance = m_terrainParams.ChunkSize / MaxVertices;

		for( int z = 0; z < row_width; ++z )
		{
			for( int x = 0; x < row_width; ++x )
			{
				const int current = z * row_width + x;
				DD_ASSERT( current < MeshVertexCount );

				// height is y
				m_vertices[ current ] = glm::vec3( x * vertex_distance, 0, z * vertex_distance );
			}
		}

		int flap_vertex_start = MeshVertexCount;
		for( int x = 0; x < row_width; ++x )
		{
			m_vertices[ flap_vertex_start + x ] = glm::vec3( x * vertex_distance, 0, 0 );
		}

		flap_vertex_start += row_width;
		for( int z = 0; z < row_width; ++z )
		{
			m_vertices[ flap_vertex_start + z ] = glm::vec3( m_terrainParams.ChunkSize, 0, z * vertex_distance );
		}

		flap_vertex_start += row_width;
		for( int x = 0; x < row_width; ++x )
		{
			m_vertices[ flap_vertex_start + x ] = glm::vec3( x * vertex_distance, 0, m_terrainParams.ChunkSize );
		}

		flap_vertex_start += row_width;
		for( int z = 0; z < row_width; ++z )
		{
			m_vertices[ flap_vertex_start + z ] = glm::vec3( 0, 0, z * vertex_distance );
		}

		m_state.TransitionTo( INITIALIZE_DONE );
	}

	void TerrainChunk::SwitchLOD( int lod )
	{
		if( m_lod != lod )
		{
			m_lod = lod;

			m_dirty = true;
		}
	}

	void TerrainChunk::SetNoiseOffset( glm::vec2 noise_offset )
	{
		if( m_offset != noise_offset )
		{
			m_offset = noise_offset;

			m_dirty = true;
		}
	}

	void TerrainChunk::Update()
	{
		if( m_state == INITIALIZE_PENDING )
		{
			m_jobsystem.Schedule( [this]() { Initialize(); } );
		}

		if( m_state == INITIALIZE_DONE )
		{
			m_state.TransitionTo( UPDATE_PENDING );
		}

		if( m_state == RENDER_UPDATE_DONE )
		{
			m_state.TransitionTo( READY );
		}

		if( m_dirty )
		{
			if( m_state == UPDATE_DONE || m_state == READY )
			{
				m_state.TransitionTo( UPDATE_PENDING );
				m_dirty = false;
			}
		}

		if( m_state == UPDATE_PENDING )
		{
			if( !m_updating.exchange( true ) )
			{
				m_jobsystem.Schedule( [this]() { UpdateVertices(); } );
			}
		}

		if( m_state == UPDATE_DONE )
		{
			m_state.TransitionTo( RENDER_UPDATE_PENDING );
		}
	}

	void TerrainChunk::RenderUpdate()
	{
		if( m_state == RENDER_UPDATE_PENDING )
		{
			if( !m_mesh.IsValid() )
			{
				CreateMesh( m_position );
			}
			else
			{
				ddr::Mesh* mesh = m_mesh.Access();
				mesh->SetIndices( s_indexBuffers[m_lod] );
				mesh->SetDirty();
			}

			m_state.TransitionTo( RENDER_UPDATE_DONE );
		}
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
			height += noise * m_terrainParams.Amplitudes[ i ];

			total_amplitude += m_terrainParams.Amplitudes[ i ];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainChunk::UpdateVertices()
	{
		DD_ASSERT( m_updating );
		DD_ASSERT( m_state == UPDATE_PENDING );

		if( m_minLod <= m_lod && m_offset == m_previousOffset )
		{
			m_state.TransitionTo( UPDATE_DONE );
			m_updating = false;
			return;
		}

		m_minLod = m_lod;
		m_previousOffset = m_offset;

		const uint stride = 1 << m_lod;
		DD_ASSERT( stride < MaxVertices );

		const int row_width = MaxVertices + 1;
		const float vertex_distance = m_terrainParams.ChunkSize / MaxVertices;

		float max_height = std::numeric_limits<float>::min();
		float min_height = std::numeric_limits<float>::max();

		for( int z = 0; z < row_width; z += stride )
		{
			for( int x = 0; x < row_width; x += stride )
			{
				const float x_coord = m_position.x + m_offset.x + x * vertex_distance;
				const float z_coord = m_position.y + m_offset.y + z * vertex_distance;

				float height = GetNoise( x_coord, z_coord );

				// height is y
				float normalized_height = (1 + height) / 2;
				DD_ASSERT( normalized_height >= 0 && normalized_height <= 1 );

				const int current = z * row_width + x;
				DD_ASSERT( current < MeshVertexCount );

				m_vertices[ current ].y = normalized_height * m_terrainParams.HeightRange;

				max_height = ddm::max( max_height, m_vertices[ current ].y );
				min_height = ddm::min( min_height, m_vertices[ current ].y );
			}
		}

		m_bounds.Min = glm::vec3( 0, min_height, 0 );
		m_bounds.Max = glm::vec3( m_terrainParams.ChunkSize, max_height, m_terrainParams.ChunkSize );

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

		DD_ASSERT( flap_vertex_start + row_width <= TotalVertexCount );

		m_state.TransitionTo( UPDATE_DONE );
		m_updating = false;
	}

	void TerrainChunk::CreateMesh( glm::vec2 pos )
	{
		DD_PROFILE_SCOPED( TerrainChunk_CreateMesh );

		std::string name = fmt::format( "terrain_{0:.2f}x{1:.2f}", pos.x, pos.y );

		m_mesh = ddr::MeshManager::Instance()->Create( name.c_str() );

		ddr::Mesh* mesh = m_mesh.Access();
		mesh->SetMaterial( s_material );
		mesh->SetPositions( m_verticesBuffer );
		mesh->SetIndices( s_indexBuffers[ m_lod ] );
		mesh->SetBoundBox( m_bounds );
	}

	void TerrainChunk::WriteHeightImage( const char* filename ) const
	{
		const int actual_vertices = MaxVertices + 1;
		byte pixels[ actual_vertices * actual_vertices ];

		for( int y = 0; y < actual_vertices; ++y )
		{
			for( int x = 0; x < actual_vertices; ++x )
			{
				pixels[ y * actual_vertices + x ] = (byte) ((m_vertices[ y * actual_vertices + x ].y / m_terrainParams.HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, actual_vertices, actual_vertices, 1, pixels );
	}

	static byte NormalToColour( float f )
	{
		return (byte) (((f + 1.0f) / 2.0f) * 255.f);
	}
}