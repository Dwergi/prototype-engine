//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PrecompiledHeader.h"
#include "ShaderProgram.h"

#include "Shader.h"

#include "GL/gl3w.h"

namespace dd
{
	ShaderProgram::ShaderProgram( const String& name )
		: m_name( name ),
		m_refCount( nullptr ),
		m_valid( true )
	{
		m_id = glCreateProgram();

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "glCreateProgram failed!" );

		m_valid = m_id != OpenGL::InvalidID;
	
		m_refCount = new std::atomic<int>( 1 );
	}

	ShaderProgram::ShaderProgram( const ShaderProgram& other ) :
		m_id( other.m_id ),
		m_name( other.m_name ),
		m_refCount( other.m_refCount ),
		m_valid( other.m_valid )
	{
		Retain();
	}

	ShaderProgram::~ShaderProgram()
	{
		Release();
	}

	ShaderProgram& ShaderProgram::operator=( const ShaderProgram& other )
	{
		Release();

		m_id = other.m_id;
		m_name = other.m_name;
		m_valid = other.m_valid;
		m_refCount = other.m_refCount;

		Retain();

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

			m_valid = false;
		}

		return msg;
	}

	bool ShaderProgram::InUse() const
	{
		GLint currentProgram = 0;
		glGetIntegerv( GL_CURRENT_PROGRAM, &currentProgram );

		return currentProgram == (GLint) m_id;
	}

	void ShaderProgram::Use( bool use ) const
	{
		DD_ASSERT( !InUse() == use, "Trying to change use state to program's current state!" );

		glUseProgram( use ? m_id : 0 );
	}

	ShaderProgram ShaderProgram::Create( const String& name, const Vector<Shader>& shaders )
	{
		ShaderProgram program( name );

		if( shaders.Size() == 0 )
			program.m_valid = false;

		DD_ASSERT_ERROR( shaders.Size() > 0, "Failed to provide any shaders to ShaderProgram!" );

		for( Shader& shader : shaders )
		{
			DD_ASSERT_ERROR( shader.IsValid(), "Invalid shader given to program!" );

			glAttachShader( program.m_id, shader.m_id );
		}

		String256 msg = program.Link();

		if( !msg.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "Linking program failed!" );
			
			program.m_valid = false;
		}

		for( Shader& shader : shaders )
		{
			glDetachShader( program.m_id, shader.m_id );
		}

		return program;
	}

	int ShaderProgram::GetAttribute( const char* name ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty attribute name given!" );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name );

		return (int) attrib;
	}

	int ShaderProgram::GetUniform( const char* name ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		return (int) uniform;
	}

	void ShaderProgram::Retain()
	{
		DD_ASSERT( m_refCount != nullptr );

		++*m_refCount;
	}

	void ShaderProgram::Release()
	{
		DD_ASSERT( m_refCount != nullptr );

		if( --*m_refCount == 0 )
		{
			if( m_id != OpenGL::InvalidID )
			{
				glDeleteProgram( m_id );
			}

			delete m_refCount;
			m_refCount = nullptr;
		}
	}
}