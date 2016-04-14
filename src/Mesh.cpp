//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "Camera.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"

namespace dd
{
	float s_unitCube[] = 
	{
		//  X     Y     Z       U     V          Normal
		// bottom
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,

		// front
		-1.0f,-1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,

		// back
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		1.0f, 1.0f,-1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,

		// left
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,

		// right
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f
	};

	Mesh::Mesh( const char* name, ShaderProgram& program ) :
		m_refCount( nullptr ),
		m_vao( OpenGL::InvalidID ),
		m_vbo( OpenGL::InvalidID ),
		m_shader( &program ),
		m_name( name )
	{
		DD_PROFILE_START( Mesh_Create );

		glGenBuffers( 1, &m_vbo );
		glGenVertexArrays( 1, &m_vao );

		glBindVertexArray( m_vao );

		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof( s_unitCube ), s_unitCube, GL_STATIC_DRAW );

		// TODO: Don't do this.
		m_shader->BindAttributeFloat( "position", 3, 8 * sizeof( float ), false );
		m_shader->BindAttributeFloat( "uv", 2, 8 * sizeof( float ), false );
		m_shader->BindAttributeFloat( "normal", 3, 8 * sizeof( float ), true );

		glBindVertexArray( 0 );

		m_refCount = new std::atomic<int>( 1 );

		DD_PROFILE_END();
	}

	Mesh::Mesh( const Mesh& other ) :
		m_refCount( other.m_refCount ),
		m_vao( other.m_vao ),
		m_vbo( other.m_vbo ),
		m_name( other.m_name ),
		m_shader( other.m_shader )
	{
		Retain();
	}

	Mesh::~Mesh()
	{
		Release();
	}

	Mesh& Mesh::operator=( const Mesh& other )
	{
		Release();

		m_refCount = other.m_refCount;
		m_vao = other.m_vao;
		m_vbo = other.m_vbo;
		m_name = other.m_name;
		m_shader = other.m_shader;

		Retain();

		return *this;
	}

	void Mesh::Render( const Camera& camera, const glm::vec3& position )
	{
		DD_PROFILE_START( Mesh_Render );

		DD_ASSERT( m_shader != nullptr );
		DD_ASSERT( m_shader->IsValid() );

		m_shader->Use( true );

		glBindVertexArray( m_vao );

		glm::mat4 model = glm::translate( position );
		glm::mat4 view = camera.GetTransform();
		glm::mat4 projection = camera.GetProjection();

		glm::mat4 mvp = projection * view * model;

		m_shader->SetUniform( "mvp", mvp );

		glDrawArrays( GL_TRIANGLES, 0, 6 * 2 * 3 );

		glBindVertexArray( 0 );

		m_shader->Use( false );

		DD_PROFILE_END();
	}

	void Mesh::Retain()
	{ 
		DD_ASSERT( m_refCount != nullptr );

		++*m_refCount;
	}

	void Mesh::Release()
	{
		DD_ASSERT( m_refCount != nullptr );

		if( --*m_refCount <= 0 )
		{
			glDeleteBuffers( 1, &m_vbo );
			glDeleteVertexArrays( 1, &m_vao );

			delete m_refCount;

			m_vbo = OpenGL::InvalidID;
			m_vao = OpenGL::InvalidID;
		}
	}

	void Mesh::AddRef()
	{
		Retain();
	}

	void Mesh::RemoveRef()
	{
		Release();
	}
}