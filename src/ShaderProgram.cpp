//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PrecompiledHeader.h"
#include "ShaderProgram.h"

#include "Shader.h"

namespace dd
{
	ShaderProgram::ShaderProgram( const String& name )
		: m_name( name )
	{
		m_id = glCreateProgram();

		ASSERT_ERROR( m_id != 0, "glCreateProgram failed!" );
	}

	ShaderProgram::ShaderProgram( ShaderProgram&& other )
		: m_id( other.m_id )
	{
		other.m_id = OpenGL::InvalidID;
	}
	
	ShaderProgram::ShaderProgram( const ShaderProgram& other )
		: m_id( other.m_id )
	{

	}

	ShaderProgram::~ShaderProgram()
	{
		Invalidate();
	}

	ShaderProgram& ShaderProgram::operator=( const ShaderProgram& other )
	{
		return *this;
	}

	String256 ShaderProgram::Link()
	{
		glLinkProgram( m_id );

		String256 msg;

		GLint status;
		glGetProgramiv( m_id, GL_LINK_STATUS, &status );
		if( status == GL_FALSE )
		{
			msg = "Program linking failure: ";

			GLint infoLogLength;
			glGetProgramiv( m_id, GL_INFO_LOG_LENGTH, &infoLogLength );

			char* strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog( m_id, infoLogLength, NULL, strInfoLog );

			msg += strInfoLog;
			delete[] strInfoLog;

			glDeleteProgram( m_id );
			m_id = OpenGL::InvalidID;
		}

		return msg;
	}

	void ShaderProgram::Invalidate()
	{
		if( m_id != OpenGL::InvalidID )
		{
			glDeleteProgram( m_id );
			m_id = OpenGL::InvalidID;
		}
	}

	ShaderProgram ShaderProgram::Create( const String& name, const Vector<Shader*>& shaders )
	{
		ASSERT_ERROR( shaders.Size() == 0, "Failed to provide any shaders to ShaderProgram!" );

		ShaderProgram program( name );

		for( Shader* shader : shaders )
		{
			ASSERT_ERROR( shader != nullptr, "Null shader given to program!" );
			ASSERT_ERROR( shader->IsValid(), "Invalid shader given to program!" );

			glAttachShader( program.m_id, shader->m_id );
		}

		String256 msg = program.Link();

		if( !msg.IsEmpty() )
		{
			ASSERT_ERROR( false, "Linking program failed!" );
			program.Invalidate();
		}

		for( Shader* shader : shaders )
		{
			glDetachShader( program.m_id, shader->m_id );
		}

		return program;
	}

	int ShaderProgram::GetAttribute( const String& name ) const
	{
		ASSERT( !name.IsEmpty(), "Empty attribute name given!" );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name.c_str() );

		ASSERT( attrib != -1, "Program attribute not found: %s", name.c_str() );

		return (int) attrib;
	}

	int ShaderProgram::GetUniform( const String& name ) const
	{
		ASSERT( !name.IsEmpty(), "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name.c_str() );

		ASSERT( uniform != -1, "Program uniform not found: %s", name.c_str() );

		return (int) uniform;
	}
}