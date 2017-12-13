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

	int TerrainChunk::s_indices[(VerticesPerDim - 1) * (VerticesPerDim - 1) * 6];

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

		for( uint16 y = 0; y < VerticesPerDim - 1; ++y )
		{
			for( uint16 x = 0; x < VerticesPerDim; ++x )
			{
				bool first = x == 0;
				bool last = x == (VerticesPerDim - 1);

				uint16 next_row = (y + 1) * VerticesPerDim + x;

				if( !last )
				{
					s_indices[index] = y * VerticesPerDim + x;
					s_indices[index + 1] = next_row;
					s_indices[index + 2] = y * VerticesPerDim + x + 1;

					index += 3;
				}

				if( !first )
				{
					s_indices[index] = y * VerticesPerDim + x;
					s_indices[index + 1] = next_row - 1;
					s_indices[index + 2] = next_row;

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
	
	void TerrainChunk::Destroy( EntityManager& entity_manager )
	{
		entity_manager.Destroy( m_entity );
	}

	void TerrainChunk::Generate( EntityManager& entity_manager )
	{
		float offset = (float) m_key.Size / (VerticesPerDim - 1);
		float height_range = HeightRange / 2; // noise returns between -1 and 1

		for( uint y = 0; y < VerticesPerDim; ++y )
		{
			for( uint x = 0; x < VerticesPerDim; ++x )
			{
				float x_coord = m_key.X + x * offset;
				float y_coord = m_key.Y + y * offset;

				float height = GetHeight( x_coord, y_coord );

				// height is y
				m_vertices[y * VerticesPerDim + x].y = (1 + height) * height_range;
				m_vertices[y * VerticesPerDim + x].x = x * offset;
				m_vertices[y * VerticesPerDim + x].z = y * offset;
			}
		}

		m_entity = entity_manager.CreateEntity<TransformComponent, MeshComponent>();
		MeshComponent* mesh_cmp = m_entity.Get<MeshComponent>().Write();

		MeshHandle mesh_h = Mesh::Create( "terrain", s_shader );
		Mesh* mesh = mesh_h.Get();
		mesh->SetData( (float*) &m_vertices[0].x, VerticesPerDim * VerticesPerDim * 3, 3 );

		mesh->BindAttribute( "Position", 3, 0, false );

		m_enabled = true;
	}

	void TerrainChunk::Write( const char* filename )
	{
		byte pixels[VerticesPerDim * VerticesPerDim];

		for( int y = 0; y < VerticesPerDim; ++y )
		{
			for( int x = 0; x < VerticesPerDim; ++x )
			{
				pixels[y * VerticesPerDim + x] = (int) ((m_vertices[y * VerticesPerDim + x].y / HeightRange) * 255);
			}
		}

		stbi_write_tga( filename, VerticesPerDim, VerticesPerDim, 1, pixels );
	}
}