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
	float TerrainChunk::HeightRange = 8.f;

	TerrainChunk::IndexBuffer<TerrainChunk::VerticesPerDim, TerrainChunk::VerticesPerDim> TerrainChunk::s_indices;

	TerrainChunk::TerrainChunk( const TerrainChunkKey& key ) :
		m_key( key ),
		m_generated( false ),
		m_created( false )
	{

	}

	TerrainChunk::~TerrainChunk()
	{
		if( !m_created )
			return;

		m_vboVertex.Destroy();
		m_vao.Destroy();
	}

	void TerrainChunk::CreateRenderResources( ShaderProgram& shader )
	{
		if( !m_generated ||	m_created )
			return;

		m_created = true;

		m_vao.Create();
		m_vao.Bind();

		s_indices.Create();
		s_indices.Bind();

		m_vboVertex.Create( GL_ARRAY_BUFFER );
		m_vboVertex.Bind();
		m_vboVertex.SetData( &m_vertices[0], sizeof( m_vertices[0] ) * VerticesPerDim * VerticesPerDim );

		m_shader = &shader;
		m_shader->BindAttributeFloat( "position", 3, 3 * sizeof( GLfloat ), false );

		m_vao.Unbind();

		CheckGLError();
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
		if( m_generated )
			return;

		m_generated = true;

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
	}

	void TerrainChunk::Render( Camera& camera )
	{
		if( !m_generated || !m_created )
			return;

		m_vao.Bind();

		m_shader->Use( true );

		glm::mat4 model = glm::translate( glm::vec3( m_key.X, 0, m_key.Y ) );

		m_shader->SetUniform( "Model", model );
		m_shader->SetUniform( "View", camera.GetCameraMatrix() );
		m_shader->SetUniform( "Projection", camera.GetProjection() );

		float clr = m_key.Size / 128.f;
		glm::vec4 colour( clr, clr, clr, 1.0 );
		m_shader->SetUniform( "colour_multiplier", colour );

		glDrawElements( GL_TRIANGLES, s_indices.Count, GL_UNSIGNED_SHORT, 0 );

		m_shader->Use( false );

		m_vao.Unbind();

		CheckGLError();
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