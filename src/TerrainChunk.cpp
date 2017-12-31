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
	float TerrainChunk::Amplitudes[Octaves] = { 0.5f, 0.3f, 0.2f, 0.1f, 0.05f, 0.025f, 0.0125f, 0.005f };

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
	
	void TerrainChunk::Destroy( EntityManager& entity_manager )
	{
		if( m_entity.IsValid() )
		{
			const MeshComponent* pMesh = m_entity.Get<MeshComponent>().Read();
			if( pMesh->Mesh.IsValid() )
			{
				Mesh::Destroy( pMesh->Mesh );
			}

			entity_manager.Destroy( m_entity );
		}
	}

	void TerrainChunk::Generate( EntityManager& entity_manager )
	{
		DD_PROFILE_START( TerrainChunk_InitializeVerts );

		for( int z = 0; z < Vertices + 1; ++z )
		{
			for( int x = 0; x < Vertices + 1; ++x )
			{
				const int current = 2 * z * (Vertices + 1) + 2 * x;

				// height is y
				m_vertices[current].y = 0;
				m_vertices[current].x = x * m_key.Size;
				m_vertices[current].z = z * m_key.Size;
			}
		}

		DD_PROFILE_END();

		DD_PROFILE_START( TerrainChunk_CreateEntity );

		m_entity = entity_manager.CreateEntity<TransformComponent, MeshComponent>();

		DD_PROFILE_END();

		DD_PROFILE_START( TerrainChunk_CreateMesh );

		char name[128];
		sprintf_s( name, 128, "%.2fx%.2f_%d", m_key.X, m_key.Y, m_key.LOD );

		MeshHandle mesh_h = Mesh::Create( name, s_shader );

		Mesh* mesh = mesh_h.Get();
		mesh->SetData( (float*) &m_vertices[0].x, sizeof( m_vertices ), 6 );
		mesh->SetIndices( s_indices, sizeof( s_indices ) / sizeof( uint ) );

		MeshComponent* mesh_cmp = m_entity.Get<MeshComponent>().Write();
		mesh_cmp->Mesh = mesh_h;

		s_shader.Get()->Use( true );

		mesh->BindAttribute( "Position", 3, 0, false );
		mesh->BindAttribute( "Normal", 3, 3, true );

		s_shader.Get()->Use( false );

		DD_PROFILE_END();
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

	void TerrainChunk::UpdateVertices( const glm::vec2& chunkPos )
	{
		DD_PROFILE_START( TerrainChunk_GenerateVerts );

		for( int z = 0; z < Vertices + 1; ++z )
		{
			for( int x = 0; x < Vertices + 1; ++x )
			{
				const float x_coord = chunkPos.x + x * m_key.Size;
				const float z_coord = chunkPos.y + z * m_key.Size;

				const int current = 2 * z * (Vertices + 1) + 2 * x;

				float height = GetHeight( x_coord, z_coord );

				// height is y
				m_vertices[current].y = ((1 + height) / 2) * HeightRange;
			}
		}

		DD_PROFILE_END();
	}

	void TerrainChunk::Update( glm::vec3& origin )
	{
		glm::vec2 chunkPos = glm::vec2( m_key.X + origin.x, m_key.Y + origin.z );

		if( chunkPos != m_lastPosition )
		{
			UpdateVertices( chunkPos );
			UpdateNormals();

			MeshComponent* mesh_cmp = m_entity.Get<MeshComponent>().Write();
			Mesh* mesh = mesh_cmp->Mesh.Get();

			mesh->UpdateData();

			AABB bounds;
			bounds.Expand( glm::vec3( 0 ) );
			bounds.Expand( glm::vec3( m_key.Size * Vertices, HeightRange, m_key.Size * Vertices ) );

			mesh->SetBounds( bounds );

			TransformComponent* transform_cmp = m_entity.Get<TransformComponent>().Write();
			transform_cmp->SetLocalPosition( glm::vec3( chunkPos.x, 0, chunkPos.y ) );

			m_lastPosition = chunkPos;
		}
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