//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainChunk.h"

#include "ICamera.h"
#include "EntityManager.h"
#include "GLError.h"
#include "JobSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TerrainChunkKey.h"
#include "TerrainParameters.h"

#include "GL/gl3w.h"

#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	uint TerrainChunk::s_indices[IndexCount];
	ddr::ShaderHandle TerrainChunk::s_shader;
	ddr::MaterialHandle TerrainChunk::s_material;

	TerrainChunk::TerrainChunk( const TerrainParameters& params, const TerrainChunkKey& key ) :
		m_params( params ),
		m_key( key )
	{
		m_verticesBuffer.Set( m_vertices, VertexCount );
		m_normalsBuffer.Set( m_normals, VertexCount );
		m_indices.Set( s_indices, IndexCount );
	}

	TerrainChunk::~TerrainChunk()
	{
		ddr::Mesh::Destroy( m_mesh );

		m_verticesBuffer.Release();
		m_normalsBuffer.Release();
	}

	void TerrainChunk::InitializeShared()
	{
		uint index = 0;
		const int actual_vertices = Vertices + 1;

		for( uint z = 0; z < Vertices; ++z )
		{
			for( uint x = 0; x < actual_vertices; ++x )
			{
				const uint current = z * actual_vertices + x;
				const uint next_row = (z + 1) * actual_vertices + x;

				DD_ASSERT( next_row < actual_vertices * actual_vertices );

				if( x != 0 )
				{
					s_indices[index] = current;
					s_indices[index + 1] = next_row - 1;
					s_indices[index + 2] = next_row;

					index += 3;
				}

				if( x != Vertices )
				{
					s_indices[index] = current;
					s_indices[index + 1] = next_row;
					s_indices[index + 2] = current + 1;

					index += 3;
				}
			}
		}

		DD_ASSERT( index == MeshIndexCount );

		int flap_vertex_start = MeshVertexCount;

		// (top, x = varying, z = 0)
		for( uint x = 0; x < Vertices; ++x )
		{
			s_indices[index + 0] = x;
			s_indices[index + 1] = flap_vertex_start + x + 1;
			s_indices[index + 2] = flap_vertex_start + x;

			index += 3;

			s_indices[index + 0] = x;
			s_indices[index + 1] = x + 1;
			s_indices[index + 2] = flap_vertex_start + x + 1;

			index += 3;
		}

		flap_vertex_start += actual_vertices;

		// 4th (right, x = chunk size, z = varying)
		for( uint z = 0; z < Vertices; ++z )
		{
			s_indices[ index + 0 ] = (z + 2) * actual_vertices - 1;
			s_indices[ index + 1 ] = flap_vertex_start + z + 1;
			s_indices[ index + 2 ] = flap_vertex_start + z;

			index += 3;

			s_indices[ index + 0 ] = flap_vertex_start + z;
			s_indices[ index + 1 ] = (z + 1) * actual_vertices - 1;
			s_indices[ index + 2 ] = (z + 2) * actual_vertices - 1;

			index += 3;
		}
		
		flap_vertex_start += actual_vertices;

		// (bottom, x = varying, z = chunk size)
		for( uint x = 0; x < Vertices; ++x )
		{
			const uint last_row = MeshVertexCount - actual_vertices;

			s_indices[index + 0] = last_row + x + 1;
			s_indices[index + 1] = flap_vertex_start + x;
			s_indices[index + 2] = flap_vertex_start + x + 1;

			index += 3;

			s_indices[index + 0] = last_row + x;
			s_indices[index + 1] = flap_vertex_start + x;
			s_indices[index + 2] = last_row + x + 1;

			index += 3;
		}

		flap_vertex_start += actual_vertices;

		// (left, x = 0, z = varying)
		for( uint z = 0; z < Vertices; ++z )
		{
			s_indices[index + 0] = (z + 1) * actual_vertices;
			s_indices[index + 1] = flap_vertex_start + z;
			s_indices[index + 2] = flap_vertex_start + z + 1;

			index += 3;

			s_indices[index + 0] = z * actual_vertices;
			s_indices[index + 1] = flap_vertex_start + z;
			s_indices[index + 2] = (z + 1) * actual_vertices;

			index += 3;
		}

		DD_ASSERT( index == IndexCount );
	}

	void TerrainChunk::CreateRenderResources()
	{
		Vector<ddr::Shader*> shaders;

		ddr::Shader* vert = ddr::Shader::Create( String8( "shaders\\standard.vertex" ), ddr::Shader::Type::Vertex );
		DD_ASSERT( vert != nullptr );
		shaders.Add( vert );

		ddr::Shader* geom = ddr::Shader::Create( String8( "shaders\\standard.geometry" ), ddr::Shader::Type::Geometry );
		DD_ASSERT( geom != nullptr );
		shaders.Add( geom );

		ddr::Shader* pixel = ddr::Shader::Create( String8( "shaders\\standard.pixel" ), ddr::Shader::Type::Pixel );
		DD_ASSERT( pixel != nullptr );
		shaders.Add( pixel );

		s_shader = ddr::ShaderProgram::Create( String8( "terrain" ), shaders );
		s_material = ddr::Material::Create( "terrain" );

		ddr::Material* material = ddr::Material::Get( s_material );
		material->SetShader( s_shader );
	}

	void TerrainChunk::Generate()
	{
		DD_PROFILE_SCOPED( TerrainChunk_Generate );

		const float actual_distance = m_params.VertexDistance * (1 << m_key.LOD);
		const int actual_vertices = Vertices + 1;
		for( int z = 0; z < actual_vertices; ++z )
		{
			for( int x = 0; x < actual_vertices; ++x )
			{
				const int current = z * actual_vertices + x;

				// height is y
				m_vertices[ current ] = glm::vec3( x * actual_distance, 0, z * actual_distance );
			}
		}

		const float chunk_size = Vertices * actual_distance;
		int flap_vertex_start = MeshVertexCount;
		for( int x = 0; x < actual_vertices; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( x * actual_distance, 0, 0 );
		}

		flap_vertex_start += actual_vertices;
		for( int x = 0; x < actual_vertices; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( chunk_size, 0, x * actual_distance );
		}

		flap_vertex_start += actual_vertices;
		for( int x = 0; x < actual_vertices; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( x * actual_distance, 0, chunk_size );
		}

		flap_vertex_start += actual_vertices;
		for( int x = 0; x < actual_vertices; ++x )
		{
			m_vertices[flap_vertex_start + x] = glm::vec3( 0, 0, x * actual_distance );
		}

		m_dataDirty = true;
	}

	void TerrainChunk::Update( JobSystem& job_system, float delta_t )
	{
		if( m_dataDirty )
		{
			job_system.Schedule(
				[this]()
			{
				UpdateVertices( m_noiseOffset ); 
				UpdateNormals();

				m_dataDirty = false;
				m_renderDirty = true;
			} );
		}
	}

	void TerrainChunk::RenderUpdate()
	{
		if( m_destroy )
		{
			ddr::Mesh::Destroy( m_mesh );
		}

		if( m_renderDirty )
		{
			if( !m_mesh.IsValid() )
			{
				CreateMesh( m_key );
			}

			ddr::Mesh* mesh = ddr::Mesh::Get( m_mesh );
			mesh->UpdateBuffers();

			float actual_distance = m_params.VertexDistance * (1 << m_key.LOD);
			float total_size = actual_distance * Vertices;

			AABB bounds;
			bounds.Expand( glm::vec3( 0 ) );
			bounds.Expand( glm::vec3( total_size, m_params.HeightRange + (1 - (m_key.LOD / 10.0f)), total_size ) );

			mesh->SetBounds( bounds );

			m_renderDirty = false;
		}
	}

	void TerrainChunk::Destroy()
	{
		m_destroy = true;
	}

	float TerrainChunk::GetHeight( float x, float y )
	{
		float height = 0;
		float wavelength = m_params.Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < m_params.Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec3 coord( x * multiplier, y * multiplier, m_params.Seed );

			float noise = glm::simplex( coord );
			height += noise * m_params.Amplitudes[i];

			total_amplitude += m_params.Amplitudes[i];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainChunk::UpdateVertices( glm::vec2 origin )
	{
		glm::vec2 chunk_pos = origin + glm::vec2( m_key.X, m_key.Y );
		const int actual_vertices = Vertices + 1;
		const float actual_distance = m_params.VertexDistance * (1 << m_key.LOD);

		for( int z = 0; z < actual_vertices; ++z )
		{
			for( int x = 0; x < actual_vertices; ++x )
			{
				const float x_coord = chunk_pos.x + x * actual_distance;
				const float z_coord = chunk_pos.y + z * actual_distance;

				const int current = z * actual_vertices + x;

				float height = GetHeight( x_coord, z_coord );

				// height is y
				float normalized_height = (1 + height) / 2;
				DD_ASSERT( normalized_height >= 0 && normalized_height <= 1 );

				m_vertices[current].y = normalized_height * m_params.HeightRange;
			}
		}

		for( int i = 0; i < FlapVertexCount; ++i )
		{
			m_vertices[MeshVertexCount + i].y = 0.0f;
		}

		int flap_vertex_start = MeshVertexCount;
		for( int i = 0; i < actual_vertices; ++i )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].z == 0.0f );
		}

		flap_vertex_start += actual_vertices;
		for( int i = 0; i < actual_vertices; ++i )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].x == Vertices * actual_distance );
		}

		flap_vertex_start += actual_vertices;
		for( int i = 0; i < actual_vertices; ++i )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].z == Vertices * actual_distance );
		}

		flap_vertex_start += actual_vertices;
		for( int i = 0; i < actual_vertices; ++i )
		{
			DD_ASSERT( m_vertices[ flap_vertex_start + i ].x == 0.0f );
		}
	}

	void TerrainChunk::UpdateNormals()
	{
		DD_PROFILE_SCOPED( TerrainChunk_InitializeNormals );

		for( int i = 0; i < MeshIndexCount; i += 3 )
		{
			uint indexA = s_indices[i];
			uint indexB = s_indices[i + 1];
			uint indexC = s_indices[i + 2];

			glm::vec3 a = m_vertices[indexA];
			glm::vec3 b = m_vertices[indexB];
			glm::vec3 c = m_vertices[indexC];

			glm::vec3 normal = glm::normalize( glm::cross( b - a, c - a ) );
			if( normal.y < 0 )
			{
				normal = glm::normalize( glm::cross( c - a, b - a ) );

				// should always be pointing at least a little bit up
				DD_ASSERT( normal.y >= 0 );
			}

			m_normals[indexA] = normal;
			m_normals[indexB] = normal;
			m_normals[indexC] = normal;
		}

		for( int i = 0; i < FlapVertexCount; ++i )
		{
			m_normals[MeshVertexCount + i] = glm::vec3( 0, 1, 0 );
		}
	}


	void TerrainChunk::SetNoiseOffset( glm::vec2 noise_offset )
	{
		m_noiseOffset = noise_offset;
		m_dataDirty = true;
	}

	void TerrainChunk::CreateMesh( const TerrainChunkKey& key )
	{
		DD_PROFILE_START( TerrainChunk_CreateMesh );

		char name[ 128 ];
		sprintf_s( name, 128, "%.2fx%.2f_%d", key.X, key.Y, key.LOD );

		m_mesh = ddr::Mesh::Create( name );

		ddr::Mesh* mesh = ddr::Mesh::Get( m_mesh );
		mesh->SetMaterial( s_material );

		ddr::Material* material = ddr::Material::Get( s_material );
		ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( material->GetShader() );

		shader->Use( true );

		mesh->SetPositions( m_verticesBuffer );
		mesh->SetNormals( m_normalsBuffer );
		mesh->SetIndices( m_indices );

		for( int i = 0; i < m_params.HeightLevelCount; ++i )
		{
			shader->SetUniform( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Colour" ).c_str(), m_params.HeightColours[i] );
			shader->SetUniform( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Cutoff" ).c_str(), m_params.HeightCutoffs[i] );
		}

		shader->SetUniform( "TerrainHeightCount", m_params.HeightLevelCount );
		shader->SetUniform( "TerrainMaxHeight", m_params.HeightRange );

		shader->Use( false );

		DD_PROFILE_END();
	}

	void TerrainChunk::WriteHeightImage( const char* filename ) const
	{
		const int actualVertices = Vertices + 1;
		byte pixels[actualVertices * actualVertices];

		for( int y = 0; y < actualVertices; ++y )
		{
			for( int x = 0; x < actualVertices; ++x )
			{
				pixels[y * actualVertices + x] = (byte) ((m_vertices[y * actualVertices + x].y / m_params.HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, actualVertices, actualVertices, 1, pixels );
	}

	static byte NormalToColour( float f )
	{
		return (byte) (((f + 1.0f) / 2.0f) * 255.f);
	}

	void TerrainChunk::WriteNormalImage( const char* filename ) const
	{
		const int actualVertices = Vertices + 1;
		byte pixels[actualVertices * actualVertices * 3];

		for( int y = 0; y < actualVertices; ++y )
		{
			for( int x = 0; x < actualVertices; ++x )
			{
				int index = y * actualVertices + x;

				glm::vec3 normal = m_normals[index];
				pixels[3 * index] = NormalToColour( normal.x );
				pixels[3 * index + 1] = NormalToColour( normal.y );
				pixels[3 * index + 2] = NormalToColour( normal.z );
			}
		}

		stbi_write_tga( filename, actualVertices, actualVertices, 3, pixels );
	}
}