//
// Shader.cpp - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PCH.h"
#include "Shader.h"

#include "File.h"
#include "OpenGL.h"

namespace ddr
{
	std::unordered_map<dd::String128, Shader*> Shader::sm_shaderCache;

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

	Shader::Shader( const dd::String& path, Type type ) :
		m_path( path ),
		m_type( type )
	{
		m_id = glCreateShader( GetOpenGLShaderType( type ) );

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "Failed to create shader!" );
	}

	Shader::~Shader()
	{
		if( m_id != OpenGL::InvalidID )
		{
			glDeleteShader( m_id );
		}
	}

	bool Shader::Reload()
	{
		dd::String256 oldSource = m_source;

		dd::String256 source;
		if( !LoadFile( m_path, source ) )
		{
			return false;
		}

		dd::String256 message = Compile( source );
		if( !message.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "'%s': Compiling failed, message:\n %s", m_path.c_str(), message.c_str() );

			m_source = oldSource;
			Compile( m_source );

			return false;
		}

		m_source = source;
		return true;
	}

	dd::String256 Shader::Compile( const dd::String& source )
	{
		DD_PROFILE_SCOPED( Shader_Compile );

		dd::String256 msg;

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

	bool Shader::LoadFile( const dd::String& path, dd::String& outSource )
	{
		dd::File file( path.c_str() );
		std::string read;
		if( !file.Read( read ) )
		{
			return false;
		}

		outSource += read;
		return true;
	}

	Shader* Shader::Create( const dd::String& path, Shader::Type type )
	{
		DD_PROFILE_SCOPED( Shader_Create );

		auto it = sm_shaderCache.find( path );
		if( it != sm_shaderCache.end() )
		{
			return it->second;
		}

		if( !dd::File::Exists( path.c_str() ) )
		{
			return nullptr;
		}
		
		Shader* shader = new Shader( path, type );
		if( shader->m_id == OpenGL::InvalidID || 
			!shader->Reload() )
		{
			DD_ASSERT( false, "Failed to load shader from '%s'!", path.c_str() );
			delete shader;
			return nullptr;
		}

		sm_shaderCache.insert( std::make_pair( dd::String128( path ), shader ) );
		return shader;
	}
}