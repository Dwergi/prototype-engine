//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PrecompiledHeader.h"
#include "ShaderProgram.h"

#include "Shader.h"

#include "GL/gl3w.h"

#include "glm/gtc/type_ptr.hpp"

namespace dd
{
	std::mutex ShaderProgram::m_instanceMutex;
	DenseMap<uint64, ShaderProgram> ShaderProgram::m_instances;

	ShaderHandle ShaderProgram::Create( const String& name, const Vector<Shader>& shaders )
	{
		DD_PROFILE_START( ShaderProgram_Create );

		uint64 hash = dd::Hash( name );

		std::lock_guard<std::mutex> lock( m_instanceMutex );

		ShaderProgram* instance = m_instances.Find( hash );
		if( instance == nullptr )
		{
			ShaderProgram program = CreateInstance( name, shaders );
			if( program.IsValid() )
			{
				m_instances.Add( hash, program );
			}
			else
			{
				hash = 0;
			}
		}

		ShaderHandle handle;
		handle.m_hash = hash;

		DD_PROFILE_END();

		return handle;
	}

	ShaderProgram* ShaderProgram::Get( ShaderHandle handle )
	{
		std::lock_guard<std::mutex> lock( m_instanceMutex );
		return m_instances.Find( handle.m_hash );
	}

	void ShaderProgram::Destroy( ShaderHandle handle )
	{
		std::lock_guard<std::mutex> lock( m_instanceMutex );
		return m_instances.Remove( handle.m_hash );
	}

	ShaderProgram::ShaderProgram( const String& name )
		: m_name( name ),
		m_refCount( nullptr )
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

	ShaderProgram ShaderProgram::CreateInstance( const String& name, const Vector<Shader>& shaders )
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

	String256 ShaderProgram::Link()
	{
		DD_PROFILE_START( ShaderProgram_Link );

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
		}
		else
		{
			m_valid = true;
		}

		DD_PROFILE_END();

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

	ShaderLocation ShaderProgram::GetAttribute( const char* name ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty attribute name given!" );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name );

		return attrib;
	}

	bool ShaderProgram::BindAttributeFloat( const char* name, uint count, uint stride, bool normalized )
	{
		ShaderLocation loc = GetAttribute( name );

		if( loc != InvalidLocation )
		{
			glVertexAttribPointer( loc, count, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, stride, NULL );
			glEnableVertexAttribArray( loc );

			return true;
		}

		return false;
	}

	ShaderLocation ShaderProgram::GetUniform( const char* name ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		return uniform;
	}

	void ShaderProgram::SetUniform( const char* name, float f ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		DD_ASSERT( uniform != -1 );
		if( uniform != -1 )
		{
			glUniform1f( uniform, f );
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec3& vec ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		DD_ASSERT( uniform != -1 );
		if( uniform != -1 )
		{
			glUniform3fv( uniform, 1, glm::value_ptr( vec ) );
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec4& vec ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		DD_ASSERT( uniform != -1 );
		if( uniform != -1 )
		{
			glUniform4fv( uniform, 1, glm::value_ptr( vec ) );
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::mat4& mat ) const
	{
		DD_ASSERT( m_valid, "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );

		DD_ASSERT( uniform != -1 );
		if( uniform != -1 )
		{
			glUniformMatrix4fv( uniform, 1, false, glm::value_ptr( mat ) );
		}
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