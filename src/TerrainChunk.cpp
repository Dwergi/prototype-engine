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
#include "Mesh.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TerrainChunkKey.h"

#include "GL/gl3w.h"

#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	float TerrainChunk::VertexDistance = 1.0f;
	float TerrainChunk::HeightRange = 64.0f;
	float TerrainChunk::Wavelength = 96.0f;
	float TerrainChunk::Seed = 0.0f;
	float TerrainChunk::Amplitudes[Octaves] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03f, 0.015f };
	glm::vec3 TerrainChunk::HeightColours[HeightLevels] = 
	{	
		glm::vec3( 0.25f, 0.8f, 0.25f ), // green
		glm::vec3( 0.25f, 0.5f, 0.25f ), // darker green
		glm::vec3( 0.6f, 0.4f, 0.1f ), // brown
		glm::vec3( 0.5f, 0.5f, 0.5f ), // grey
		glm::vec3( 0.9f, 0.9f, 0.9f ) // white
	};

	uint TerrainChunk::s_indices[IndexCount];
	bool TerrainChunk::UseDebugColours = false;

	ShaderHandle TerrainChunk::s_shader;

	Buffer<uint> TerrainChunk::s_bufferIndices( s_indices, IndexCount );

	TerrainChunk::TerrainChunk() 
	{

	}

	TerrainChunk::~TerrainChunk()
	{
	}

	void TerrainChunk::InitializeShared()
	{
		uint index = 0;

		for( uint y = 0; y < Vertices; ++y )
		{
			for( uint x = 0; x < Vertices + 1; ++x )
			{
				bool first = x == 0;
				bool last = x == Vertices;

				const uint next_row = (y + 1) * (Vertices + 1) + x;
				const uint current = y * (Vertices + 1) + x;

				DD_ASSERT( next_row < (Vertices + 1) * (Vertices + 1) );

				if( !first )
				{
					s_indices[index] = current;
					s_indices[index + 1] = next_row - 1;
					s_indices[index + 2] = next_row;

					index += 3;
				}

				if( !last )
				{
					s_indices[index] = current;
					s_indices[index + 1] = next_row;
					s_indices[index + 2] = current + 1;

					index += 3;
				}
			}
		}
	}

	void TerrainChunk::CreateRenderResources()
	{
		Vector<Shader> shaders;

		Shader vert = Shader::Create( String8( "shaders\\standard.vertex" ), Shader::Type::Vertex );
		DD_ASSERT( vert.IsValid() );
		shaders.Add( vert );

		Shader geom = Shader::Create( String8( "shaders\\standard.geometry" ), Shader::Type::Geometry );
		DD_ASSERT( geom.IsValid() );
		shaders.Add( geom );

		Shader pixel = Shader::Create( String8( "shaders\\standard.pixel" ), Shader::Type::Pixel );
		DD_ASSERT( pixel.IsValid() );
		shaders.Add( pixel );

		s_shader = ShaderProgram::Create( String8( "terrain" ), shaders );

		ShaderProgram* shader = s_shader.Get();
		shader->Use( true );

		shader->SetPositionsName( "Position" );
		shader->SetNormalsName( "Normal" );

		shader->Use( false );
	}

	void TerrainChunk::Generate( const TerrainChunkKey& key )
	{
		DD_PROFILE_START( TerrainChunk_InitializeVerts );

		if( m_vertices.Get() == nullptr )
		{
			m_vertices.Set( new glm::vec3[VertexCount], VertexCount );
		}

		if( m_normals.Get() == nullptr )
		{
			m_normals.Set( new glm::vec3[VertexCount], VertexCount );
		}

		const float actual_distance = VertexDistance * (1 << key.LOD);
		const int actual_vertices = Vertices + 1;
		for( int z = 0; z < actual_vertices; ++z )
		{
			for( int x = 0; x < actual_vertices; ++x )
			{
				const int current = z * actual_vertices + x;

				// height is y
				m_vertices[current].y = 0;
				m_vertices[current].x = x * actual_distance;
				m_vertices[current].z = z * actual_distance;
			}
		}

		DD_PROFILE_END();
	}

	void TerrainChunk::UpdateNormals()
	{
		for( int i = 0; i < IndexCount; i += 3 )
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
	}

	float TerrainChunk::GetHeight( float x, float y )
	{
		float height = 0;
		float wavelength = Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec3 coord( x * multiplier, y * multiplier, Seed );

			float noise = glm::simplex( coord );
			height += noise * Amplitudes[i];

			total_amplitude += Amplitudes[i];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainChunk::UpdateVertices( const TerrainChunkKey& key, const glm::vec2& origin )
	{
		glm::vec2 chunk_pos = origin + glm::vec2( key.X, key.Y );
		const int actual_vertices = Vertices + 1;
		const float actual_distance = VertexDistance * (1 << key.LOD);

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

				m_vertices[current].y = normalized_height * HeightRange;
			}
		}
	}

	void TerrainChunk::SetOrigin( const TerrainChunkKey& key, glm::vec2 origin )
	{
		UpdateVertices( key, origin );
		UpdateNormals();

		m_dirty = true;
	}

	void TerrainChunk::CreateMesh( const TerrainChunkKey& key )
	{
		DD_PROFILE_START( TerrainChunk_CreateMesh );

		char name[ 128 ];
		sprintf_s( name, 128, "%.2fx%.2f_%d", key.X, key.Y, key.LOD );

		m_mesh = Mesh::Create( name, s_shader );

		Mesh* mesh = m_mesh.Get();
		mesh->UseShader( true );

		mesh->SetPositions( m_vertices );

		mesh->EnableNormals( true );
		mesh->SetNormals( m_normals );

		mesh->EnableIndices( true );
		mesh->SetIndices( s_bufferIndices );

		mesh->UseShader( false );

		DD_PROFILE_END();
	}

	void TerrainChunk::RenderUpdate( const TerrainChunkKey& key )
	{
		if( m_dirty )
		{
			if( !m_mesh.IsValid() )
			{
				CreateMesh( key );
			}

			Mesh* mesh = m_mesh.Get();
			mesh->UpdateBuffers();

			float actual_distance = VertexDistance * (1 << key.LOD);
			float total_size = actual_distance * Vertices;

			AABB bounds;
			bounds.Expand( glm::vec3( 0 ) );
			bounds.Expand( glm::vec3( total_size, HeightRange + (1 - (key.LOD / 10.0f)), total_size ) );

			mesh->SetBounds( bounds );

			m_dirty = false;
		}
	}

	void TerrainChunk::Destroy()
	{
		m_destroy = true;
	}

	void TerrainChunk::Write( const char* filename )
	{
		const int actualVertices = Vertices + 1;
		byte pixels[actualVertices * actualVertices];

		for( int y = 0; y < actualVertices; ++y )
		{
			for( int x = 0; x < actualVertices; ++x )
			{
				pixels[y * actualVertices + x] = (byte) ((m_vertices[y * actualVertices + x].y / HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, actualVertices, actualVertices, 1, pixels );
	}

	static byte NormalToColour( float f )
	{
		return (byte) (((f + 1.0f) / 2.0f) * 255.f);
	}

	void TerrainChunk::WriteNormals( const char* filename )
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