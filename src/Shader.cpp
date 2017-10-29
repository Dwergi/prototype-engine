//
// Shader.cpp - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Shader.h"

#include "File.h"

#include "GL/gl3w.h"

namespace dd
{
	DenseMap<String128, String256> Shader::sm_shaderCache;

	GLenum GetOpenGLShaderType( Shader::Type type )
	{
		switch( type )
		{
		case Shader::Type::Vertex:
			return GL_VERTEX_SHADER;

		case Shader::Type::Pixel:
			return GL_FRAGMENT_SHADER;

		case Shader::Type::Geometry:
			return GL_GEOMETRY_SHADER;
		}

		return GL_INVALID_INDEX;
	}

	Shader::Shader( const String& name, Type type ) :
		m_name( name ),
		m_refCount( nullptr ),
		m_valid( false )
	{
		m_id = glCreateShader( GetOpenGLShaderType( type ) );

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "Failed to create shader %s", name.c_str() );

		m_valid = m_id != OpenGL::InvalidID;
		
		m_refCount = new std::atomic<int>( 1 );
	}

	Shader::Shader( const Shader& other ) :
		m_name( other.m_name ),
		m_id( other.m_id ),
		m_refCount( other.m_refCount ),
		m_valid( other.m_valid )
	{
		Retain();
	}

	Shader::~Shader()
	{
		Release();
	}

	Shader& Shader::operator=( const Shader& other )
	{
		Release();

		m_id = other.m_id;
		m_name = other.m_name;
		m_refCount = other.m_refCount;

		Retain();

		return *this;
	}

	String256 Shader::Compile( const String& source )
	{
		DD_PROFILE_SCOPED( Shader_Compile );

		String256 msg;

		const char* src = source.c_str();
		glShaderSource( m_id, 1, (const GLchar**) &src, NULL );

		glCompileShader( m_id );

		GLint status;
		glGetShaderiv( m_id, GL_COMPILE_STATUS, &status );

		if( status == GL_FALSE )
		{
			GLint infoLogLength;
			glGetShaderiv( m_id, GL_INFO_LOG_LENGTH, &infoLogLength );

			char* strInfoLog = new char[infoLogLength + 1];
			glGetShaderInfoLog( m_id, infoLogLength, NULL, strInfoLog );

			msg += strInfoLog;
			delete[] strInfoLog;

			m_valid = false;
		}

		return msg;
	}

	bool Shader::LoadFile( const String& path, String& outSource )
	{
		String256* ptr = sm_shaderCache.Find( path );
		if( ptr != nullptr )
		{
			outSource = *ptr;
			return true;
		}

		std::unique_ptr<File> file = File::OpenDataFile( path, File::Mode::Read );
		if( file == nullptr )
		{
			return false;
		}

		char buffer[2048];
		int read = 0;
		while( (read = file->Read( (byte*) buffer, 2048 )) == 2048 )
		{
			outSource.Append( buffer, 2048 );
		}

		outSource.Append( buffer, (uint) read );

		sm_shaderCache.Add( path, outSource );

		return true;
	}

	Shader Shader::Create( const String& name, const String& path, Shader::Type type )
	{
		DD_PROFILE_SCOPED( Shader_Create );

		String256 source;
		if( !LoadFile( path, source ) )
		{
			DD_ASSERT_ERROR( false, "Failed to load shader from path!" );
		}

		Shader shader( name, type );
		if( shader.m_id == OpenGL::InvalidID )
		{
			DD_ASSERT_ERROR( false, "Shader creation failed!" );
		}

		String256 message = shader.Compile( source );
		if( !message.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "Linking program failed!" );
		}

		return shader;
	}

	void Shader::Retain()
	{
		DD_ASSERT( m_refCount != nullptr );

		++*m_refCount;
	}

	void Shader::Release()
	{
		DD_ASSERT( m_refCount != nullptr );

		if( --*m_refCount <= 0 )
		{
			if( m_id != OpenGL::InvalidID )
			{
				glDeleteShader( m_id );
			}

			delete m_refCount;
			m_refCount = nullptr;
		}
	}
}