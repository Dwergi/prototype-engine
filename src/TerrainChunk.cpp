//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainChunk.h"

#include "Camera.h"
#include "EntityManager.h"
#include "GLError.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "GL/gl3w.h"

#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	float TerrainChunk::HeightRange = 8.f;
	float TerrainChunk::Wavelength = 128.0;
	float TerrainChunk::Amplitudes[Octaves];

	uint TerrainChunk::s_indices[IndexCount];

	ShaderHandle TerrainChunk::s_shader;

	TerrainChunk::TerrainChunk( const TerrainChunkKey& key ) :
		m_key( key )
	{

	}

	TerrainChunk::~TerrainChunk()
	{
	}

	float TerrainChunk::GetHeight( float x, float y )
	{
		float height = 0;
		float amplitude = 0.5f;
		float wavelength = 128.0f;

		float total_amplitude = 0;

		for( int i = 0; i < 8; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec2 coord( x * multiplier, y * multiplier );

			float noise = glm::simplex( coord );
			
			height += noise * amplitude;

			total_amplitude += amplitude;

			amplitude /= 2;
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

		Shader vert = Shader::Create( String8( "vertex" ), String8( "shaders\\terrain_vertex.glsl" ), Shader::Type::Vertex );
		DD_ASSERT( vert.IsValid() );
		shaders.Add( vert );

		Shader geom = Shader::Create( String8( "geometry" ), String8( "shaders\\terrain_geometry.glsl" ), Shader::Type::Geometry );
		DD_ASSERT( geom.IsValid() );
		shaders.Add( geom );

		Shader pixel = Shader::Create( String8( "pixel" ), String8( "shaders\\pixel.glsl" ), Shader::Type::Pixel );
		DD_ASSERT( pixel.IsValid() );
		shaders.Add( pixel );

		s_shader = ShaderProgram::Create( String8( "terrain" ), shaders );
	}
	
	void TerrainChunk::Destroy( EntityManager& entity_manager )
	{
		entity_manager.Destroy( m_entity );
	}

	void TerrainChunk::Generate( EntityManager& entity_manager )
	{
		DD_PROFILE_START( TerrainChunk_GenerateVerts );

		const float heightRange = HeightRange / 2; // noise returns between -1 and 1

		for( int z = 0; z < Vertices + 1; ++z )
		{
			for( int x = 0; x < Vertices + 1; ++x )
			{
				const float x_coord = m_key.X + x * m_key.Size;
				const float z_coord = m_key.Y + z * m_key.Size;

				const int current = z * (Vertices + 1) + x;

				float height = GetHeight( x_coord, z_coord );

				// height is y
				m_vertices[current].y = (1 + height) * heightRange;
				m_vertices[current].x = x * m_key.Size;
				m_vertices[current].z = z * m_key.Size;
			}
		}
		
		DD_PROFILE_END();

		DD_PROFILE_START( TerrainChunk_CreateEntity );

		m_entity = entity_manager.CreateEntity<TransformComponent, MeshComponent>();
		MeshComponent* mesh_cmp = m_entity.Get<MeshComponent>().Write();

		TransformComponent* transform_cmp = m_entity.Get<TransformComponent>().Write();
		transform_cmp->SetLocalPosition( glm::vec3( m_key.X, 0, m_key.Y ) );

		DD_PROFILE_END();

		DD_PROFILE_START( TerrainChunk_CreateMesh );

		char name[128];
		sprintf_s( name, 128, "%.2fx%.2f_%d", m_key.X, m_key.Y, m_key.LOD );

		MeshHandle mesh_h = Mesh::Create( name, s_shader );

		Mesh* mesh = mesh_h.Get();
		mesh->SetData( (float*) &m_vertices[0].x, sizeof( m_vertices ), 3 );
		mesh->SetIndices( s_indices, sizeof( s_indices ) / sizeof( uint ) );

		mesh_cmp->Mesh = mesh_h;

		s_shader.Get()->Use( true );

		mesh->BindAttribute( "Position", 3, 0, false );

		s_shader.Get()->Use( false );

		DD_PROFILE_END();
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