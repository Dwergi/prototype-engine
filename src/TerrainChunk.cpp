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
	float TerrainChunk::HeightRange = 8.f;
	float TerrainChunk::Wavelength = 128.0;
	float TerrainChunk::Amplitudes[Octaves] = { 0.5f, 0.3f, 0.2f, 0.1f, 0.05f, 0.025f };

	uint TerrainChunk::s_indices[IndexCount];
	bool TerrainChunk::UseDebugColours = false;

	ShaderHandle TerrainChunk::s_shader;

	TerrainChunk::TerrainChunk() 
	{

	}

	TerrainChunk::~TerrainChunk()
	{
	}

	float TerrainChunk::GetHeight( float x, float y )
	{
		float height = 0;
		float wavelength = Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec2 coord( x * multiplier, y * multiplier );

			float noise = glm::simplex( coord );
			
			height += noise * Amplitudes[i];

			total_amplitude += Amplitudes[i];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void TerrainChunk::GenerateSharedResources()
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

		Vector<Shader> shaders;

		Shader vert = Shader::Create( String8( "vertex" ), String8( "shaders\\vertex.glsl" ), Shader::Type::Vertex );
		DD_ASSERT( vert.IsValid() );
		shaders.Add( vert );

		Shader geom = Shader::Create( String8( "geometry" ), String8( "shaders\\geometry.glsl" ), Shader::Type::Geometry );
		DD_ASSERT( geom.IsValid() );
		shaders.Add( geom );

		Shader pixel = Shader::Create( String8( "pixel" ), String8( "shaders\\pixel.glsl" ), Shader::Type::Pixel );
		DD_ASSERT( pixel.IsValid() );
		shaders.Add( pixel );

		s_shader = ShaderProgram::Create( String8( "terrain" ), shaders );
	}

	MeshHandle TerrainChunk::Generate( const TerrainChunkKey& key )
	{
		DD_PROFILE_START( TerrainChunk_InitializeVerts );

		for( int z = 0; z < Vertices + 1; ++z )
		{
			for( int x = 0; x < Vertices + 1; ++x )
			{
				const int current = 2 * z * (Vertices + 1) + 2 * x;

				// height is y
				m_vertices[current].y = 0;
				m_vertices[current].x = x * key.Size;
				m_vertices[current].z = z * key.Size;
			}
		}

		DD_PROFILE_END();

		DD_PROFILE_START( TerrainChunk_CreateMesh );

		char name[128];
		sprintf_s( name, 128, "%.2fx%.2f_%d", key.X, key.Y, key.LOD );

		m_mesh = Mesh::Create( name, s_shader );

		Mesh* mesh = m_mesh.Get();
		mesh->SetData( (float*) &m_vertices[0].x, sizeof( m_vertices ), 6 );
		mesh->SetIndices( s_indices, sizeof( s_indices ) / sizeof( uint ) );

		AABB bounds;
		bounds.Expand( glm::vec3( 0 ) );
		bounds.Expand( glm::vec3( key.Size * Vertices, HeightRange, key.Size * Vertices ) );
		mesh->SetBounds( bounds );
		
		s_shader.Get()->Use( true );

		mesh->BindAttribute( "Position", 3, 0, false );
		mesh->BindAttribute( "Normal", 3, 3, true );

		s_shader.Get()->Use( false );

		DD_PROFILE_END();

		return m_mesh;
	}

	void TerrainChunk::UpdateNormals()
	{
		for( int i = 0; i < IndexCount; i += 3 )
		{
			uint indexA = s_indices[i] * 2;
			uint indexB = s_indices[i + 1] * 2;
			uint indexC = s_indices[i + 2] * 2;

			glm::vec3 a = m_vertices[indexA];
			glm::vec3 b = m_vertices[indexB];
			glm::vec3 c = m_vertices[indexC];

			glm::vec3 normal = glm::normalize( glm::cross( b - a, c - a ) );
			if( normal.y < 0 )
			{
				normal = glm::normalize( glm::cross( c - a, b - a ) );

				// should always be pointing at least a little bit up
				DD_ASSERT( normal.y > 0 );
			}

			m_vertices[indexA + 1] = normal;
			m_vertices[indexB + 1] = normal;
			m_vertices[indexC + 1] = normal;
		}
	}

	void TerrainChunk::UpdateVertices( const TerrainChunkKey& key, const glm::vec2& origin )
	{
		glm::vec2 chunk_pos = origin + glm::vec2( key.X, key.Y );

		for( int z = 0; z < Vertices + 1; ++z )
		{
			for( int x = 0; x < Vertices + 1; ++x )
			{
				const float x_coord = chunk_pos.x + x * key.Size;
				const float z_coord = chunk_pos.y + z * key.Size;

				const int current = 2 * z * (Vertices + 1) + 2 * x;

				float height = GetHeight( x_coord, z_coord );

				// height is y
				m_vertices[current].y = ((1 + height) / 2) * HeightRange;
			}
		}
	}

	void TerrainChunk::SetOrigin( const TerrainChunkKey& key, glm::vec2 origin )
	{
		UpdateVertices( key, origin );
		UpdateNormals();

		Mesh* mesh = m_mesh.Get();
		mesh->UpdateData();

		AABB bounds;
		bounds.Expand( glm::vec3( 0 ) );
		bounds.Expand( glm::vec3( key.Size * Vertices, HeightRange + (1 - (key.LOD / 10.0f)), key.Size * Vertices ) );

		mesh->SetBounds( bounds );
	}

	void TerrainChunk::Destroy()
	{
		Mesh::Destroy( m_mesh );
	}

	void TerrainChunk::Write( const char* filename )
	{
		byte pixels[Vertices * Vertices];

		for( int y = 0; y < Vertices; ++y )
		{
			for( int x = 0; x < Vertices; ++x )
			{
				pixels[y * Vertices + x] = (int) ((m_vertices[y * Vertices + x].y / HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, Vertices, Vertices, 1, pixels );
	}
}