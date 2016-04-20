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
#include "glm/gtx/transform.hpp"

#include "stb/stb_image_write.h"

namespace dd
{
	template <>
	uint64 Hash( const ChunkKey& key )
	{
		// Hash the bottom 28 bytes of the X and Y coordinates, and the inverse of the bottom 8 bytes of the size
		// This should mean that in most cases chunks are ordered by X, then Y, then inverse size
		return (key.X << 36) + (key.Y << 8) + ~(key.Size & 0xff);
	}

	bool operator<( const ChunkKey& a, const ChunkKey& b )
	{
		return Hash( a ) < Hash( b );
	}

	float TerrainChunk::HeightRange = 8.f;
	VBO TerrainChunk::m_vboIndex;
	std::mutex TerrainChunk::m_indexMutex;

	const uint s_indexCount = (TerrainChunk::VerticesPerDim - 1) * (TerrainChunk::VerticesPerDim - 1) * 6;
	unsigned short* s_indexData = nullptr;

	void CreateIndices( unsigned short* indices, uint width, uint height )
	{
		unsigned int index = 0;

		for( unsigned short y = 0; y < height - 1; ++y )
		{
			for( unsigned short x = 0; x < width; ++x )
			{
				bool first = x == 0;
				bool last = x == (width - 1);

				unsigned short next_row = (y + 1) * width + x;

				if( !last )
				{
					indices[index]		= y * width + x;
					indices[index + 1]	= next_row;
					indices[index + 2]	= y * width + x + 1;

					index += 3;
				}

				if( !first )
				{
					indices[index]		= y * width + x;
					indices[index + 1]	= next_row - 1;
					indices[index + 2]	= next_row;

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

	void TerrainChunk::CreateRenderResources( ShaderProgram& shader )
	{
		if( !m_generated )
			return;

		if( m_vao.IsValid() )
			return;

		m_vao.Create();
		m_vao.Bind();

		std::lock_guard<std::mutex> lock( m_indexMutex );

		if( s_indexData == nullptr )
		{
			s_indexData = new unsigned short[s_indexCount];
			CreateIndices( s_indexData, VerticesPerDim, VerticesPerDim );

			m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER );
			m_vboIndex.Bind();
			m_vboIndex.SetData( s_indexData, sizeof( unsigned short ) * s_indexCount );
		}

		m_vboIndex.Bind();

		m_vboVertex.Create( GL_ARRAY_BUFFER );
		m_vboVertex.Bind();
		m_vboVertex.SetData( m_vertices, sizeof( glm::vec3 ) * VerticesPerDim * VerticesPerDim );

		m_shader = &shader;
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

	void TerrainChunk::Generate()
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
		
		m_generated = true;
	}

	void TerrainChunk::Render( Camera& camera )
	{
		if( !m_generated )
			return;

		if( !m_vao.IsValid() )
			return;

		m_vao.Bind();
		m_vboVertex.Bind();
		m_vboIndex.Bind();
		
		m_shader->BindAttributeFloat( "position", 3, 3 * sizeof( GLfloat ), false );
		m_shader->Use( true );

		glm::mat4 model = glm::translate( glm::vec3( m_key.X, 0, m_key.Y ) );

		glm::mat4 mvp = camera.GetProjection() * camera.GetCameraMatrix() * model;
		m_shader->SetUniform( "mvp", mvp );

		float clr = m_key.Size / 128.f;
		glm::vec4 colour( clr, clr, clr, 1.0 );
		m_shader->SetUniform( "colour_multiplier", colour );

		glDrawElements( GL_TRIANGLES, s_indexCount, GL_UNSIGNED_SHORT, 0 );

		m_shader->Use( false );

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