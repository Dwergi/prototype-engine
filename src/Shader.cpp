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
	std::unordered_map<String128, Shader*> Shader::sm_shaderCache;

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

	Shader::Shader( const String& path, Type type ) :
		m_refCount( nullptr )
	{
		m_id = glCreateShader( GetOpenGLShaderType( type ) );

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "Failed to create shader!" );

		m_valid = m_id != OpenGL::InvalidID;
		m_refCount = new std::atomic<int>( 1 );
		m_type = type;
	}

	Shader::Shader( const Shader& other ) :
		m_id( other.m_id ),
		m_refCount( other.m_refCount ),
		m_path( other.m_path ),
		m_type( other.m_type )
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
		m_refCount = other.m_refCount;
		m_path = other.m_path;
		m_type = other.m_type;
		m_source = other.m_source;

		Retain();

		return *this;
	}

	bool Shader::Reload()
	{
		String256 oldSource = m_source;

		String256 source;
		if( !LoadFile( m_path, source ) )
		{
			DD_ASSERT_ERROR( false, "Failed to load shader from path!" );
			return false;
		}

		String256 message = Compile( source );
		if( !message.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "Compiling shader failed, message: %s", message.c_str() );

			m_source = oldSource;
			Compile( m_source );

			return false;
		}

		m_source = source;
		return true;
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
		}

		return msg;
	}

	bool Shader::LoadFile( const String& path, String& outSource )
	{
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

		return true;
	}

	Shader* Shader::Create( const String& path, Shader::Type type )
	{
		DD_PROFILE_SCOPED( Shader_Create );

		auto it = sm_shaderCache.find( path );
		if( it != sm_shaderCache.end() )
		{
			it->second->Retain();
			return it->second;
		}
		
		Shader* shader = new Shader( path, type );
		if( !shader->Reload() )
		{
			delete shader;
			return nullptr;
		}

		sm_shaderCache.insert( std::make_pair( path, shader ) );
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