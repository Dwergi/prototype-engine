//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "Camera.h"

#include "GL/gl3w.h"

#include "glm/gtc/type_ptr.hpp"

namespace dd
{
	GLfloat s_unitCube[] = 
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

	Mesh::Mesh()
	{

	}

	Mesh::~Mesh()
	{
		glDeleteBuffers( 1, &m_vbo );
		glDeleteVertexArrays( 1, &m_vao );
	}

	void Mesh::Create( ShaderProgram& program )
	{
		DD_PROFILE_START( Mesh_Create );

		m_shader = &program;

		glGenBuffers( 1, &m_vbo );
		glGenVertexArrays( 1, &m_vao );

		glBindVertexArray( m_vao );

		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBufferData( GL_ARRAY_BUFFER, sizeof( s_unitCube ), s_unitCube, GL_STATIC_DRAW );

		glVertexAttribPointer( program.GetAttribute( "position" ), 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), NULL );
		glEnableVertexAttribArray( program.GetAttribute( "position" ) );

		int uv = program.GetAttribute( "uv" );
		if( uv != -1 )
		{
			glVertexAttribPointer( uv, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), NULL );
			glEnableVertexAttribArray( uv );
		}

		int normals = program.GetAttribute( "normal" );
		if( normals != -1 )
		{
			glVertexAttribPointer( normals, 3, GL_FLOAT, GL_TRUE, 8 * sizeof( GLfloat ), NULL );
			glEnableVertexAttribArray( normals );
		}

		glBindVertexArray( 0 );

		DD_PROFILE_END();
	}

	void Mesh::Render( Camera& camera )
	{
		DD_PROFILE_START( Mesh_Render );

		DD_ASSERT( m_shader != nullptr );

		m_shader->Use( true );

		glBindVertexArray( m_vao );

		glm::mat4 model;

		glm::mat4 view = camera.GetTransform();

		glm::mat4 projection = camera.GetProjection();

		glm::mat4 mvp = projection * view * model;

		glUniformMatrix4fv( m_shader->GetUniform( "mvp" ), 1, false, glm::value_ptr( mvp ) );

		glDrawArrays( GL_TRIANGLES, 0, 6 * 2 * 3 );

		glBindVertexArray( 0 );

		m_shader->Use( false );

		DD_PROFILE_END();
	}
}