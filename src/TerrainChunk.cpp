//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainChunk.h"

#include "Camera.h"
#include "GLError.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"

#include "GL/gl3w.h"

#include "glm/gtc/noise.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	template <>
	uint64 Hash( const ChunkKey& key )
	{
		// Hash the bottom 28 bytes of the X and Y coordinates, and the inverse of the bottom 8 bytes of the LOD level
		// This should mean that in most cases chunks are ordered by X, then Y, then LOD
		return (key.X << 36) + (key.Y << 8) + (key.LOD & 0xff);
	}

	bool operator<( const ChunkKey& a, const ChunkKey& b )
	{
		return Hash( a ) < Hash( b );
	}

	float TerrainChunk::HeightRange = 8.f;
	VBO TerrainChunk::m_vboIndex;

	const uint s_indexCount = (TerrainChunk::VerticesPerDim - 1) * (TerrainChunk::VerticesPerDim - 1) * 6;
	unsigned short* s_indexData = nullptr;

	void CreateIndices( unsigned short* indices )
	{
		unsigned int index = 0;

		unsigned int vertex_count = TerrainChunk::VerticesPerDim;

		for( unsigned short y = 0; y < vertex_count - 1; ++y )
		{
			for( unsigned short x = 0; x < vertex_count; ++x )
			{
				bool first = x == 0;
				bool last = x == (vertex_count - 1);

				unsigned short next_row = (y + 1) * vertex_count + x;

				if( !last )
				{
					indices[index] = y * vertex_count + x;
					indices[index + 1] = next_row;
					indices[index + 2] = y * vertex_count + x + 1;

					index += 3;
				}

				if( !first )
				{
					indices[index] = y * vertex_count + x;
					indices[index + 1] = next_row - 1;
					indices[index + 2] = next_row;

					index += 3;
				}
			}
		}
	}

	TerrainChunk::TerrainChunk( const ChunkKey& key ) :
		m_key( key )
	{

	}

	TerrainChunk::~TerrainChunk()
	{

	}

	void TerrainChunk::CreateRenderResources()
	{
		m_vao.Create();
		m_vao.Bind();

		if( s_indexData == nullptr )
		{
			s_indexData = new unsigned short[s_indexCount];
			CreateIndices( s_indexData );

			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER );
			m_vboIndex.Bind();
			m_vboIndex.SetData( s_indexData, sizeof( unsigned short ) * s_indexCount );
		}

		m_vboVertex.Create( GL_ARRAY_BUFFER );
		m_vboVertex.Bind();
		m_vboVertex.SetData( m_vertices, sizeof( glm::vec3 ) * VerticesPerDim * VerticesPerDim );
	}

	float TerrainChunk::GetHeight( float x, float y, float z )
	{
		float height = 0;
		float amplitude = 0.5f;
		float wavelength = 128.0f;

		const float coord_fudge = 0.001f;
		const float coord_multiplier = 0.25f;

		for( int i = 0; i < 8; ++i )
		{
			glm::vec3 coord( coord_fudge + x * coord_multiplier, coord_fudge + y * coord_multiplier, z / wavelength );

			float noise = glm::simplex( coord );
			
			height += noise * amplitude;

			amplitude /= 2;
			wavelength /= 2;
		}

		return height;
	}

	void TerrainChunk::Generate()
	{
		float offset = (float) m_key.Size / VerticesPerDim;
		float height_range = HeightRange / 2; // noise returns between -1 and 1

		for( uint y = 0; y < VerticesPerDim; ++y )
		{
			for( uint x = 0; x < VerticesPerDim; ++x )
			{
				float x_coord = m_key.X + x * offset;
				float y_coord = m_key.Y + y * offset;

				float height = GetHeight( x_coord, y_coord, 2.0f );

				// height is y
				m_vertices[y * VerticesPerDim + x].y = (1 + height) * height_range;
				m_vertices[y * VerticesPerDim + x].x = x_coord;
				m_vertices[y * VerticesPerDim + x].z = y_coord;
			}
		}
	}

	void TerrainChunk::Render( Camera& camera, ShaderProgram& shader )
	{
		m_vao.Bind();
		m_vboVertex.Bind();
		m_vboIndex.Bind();

		shader.BindAttributeFloat( "position", 3, 3 * sizeof( GLfloat ), false );
		shader.Use( true );

		glm::mat4 mvp = camera.GetProjection() * camera.GetCameraMatrix();
		shader.SetUniform( "mvp", mvp );

		bool light = true;

		for( uint64 i = 0; i < s_indexCount; )
		{
			glm::vec4 colour( 0.3, 0.3, 0.3, 1.0 );

			if( light )
				colour *= 2;

			shader.SetUniform( "colour_multiplier", colour );

			glDrawElements( GL_TRIANGLES, VerticesPerDim * 6, GL_UNSIGNED_SHORT, (const void*) i );

			i += VerticesPerDim * 3;

			light = !light;
		}

		shader.Use( false );

		m_vao.Unbind();
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