//
// Shader.cpp - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PCH.h"
#include "ShaderPart.h"

#include "File.h"
#include "OpenGL.h"
#include "Uniforms.h"

namespace ddr
{
	std::unordered_map<std::string, ShaderPart*> ShaderPart::sm_shaderCache;

	GLenum GetOpenGLShaderType( ShaderPart::Type type )
	{
		switch( type )
		{
		case ShaderPart::Type::Vertex:
			return GL_VERTEX_SHADER;

		case ShaderPart::Type::Pixel:
			return GL_FRAGMENT_SHADER;

		case ShaderPart::Type::Geometry:
			return GL_GEOMETRY_SHADER;
		}

		return GL_INVALID_INDEX;
	}

	ShaderPart::ShaderPart( const std::string& path, Type type ) :
		m_path( path ),
		m_type( type )
	{
		m_id = glCreateShader( GetOpenGLShaderType( type ) );

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "Failed to create shader!" );
	}

	ShaderPart::~ShaderPart()
	{
		if( m_id != OpenGL::InvalidID )
		{
			glDeleteShader( m_id );
		}
	}

	bool ShaderPart::Reload()
	{
		m_uniforms.Clear();

		std::string oldSource = m_source;

		std::string source;
		if( !LoadFile( m_path, source ) )
		{
			DD_ASSERT_ERROR( false, "'%s': Failed to load.", m_path.c_str() );
			return false;
		}

		std::string message = Compile( source );
		if( !message.empty() )
		{
			DD_ASSERT_ERROR( false, "'%s': Compiling failed, message:\n %s", m_path.c_str(), message.c_str() );

			m_source = oldSource;
			Compile( m_source );

			GatherUniforms( m_source, m_uniforms );
			return false;
		}

		m_source = source;

		GatherUniforms( m_source, m_uniforms );
		return true;
	}

	std::string ShaderPart::Compile( const std::string& source )
	{
		DD_PROFILE_SCOPED( Shader_Compile );

		std::string msg;

		const char* src = source.c_str();
		glShaderSource( m_id, 1, (const GLchar**) &src, NULL );

		glCompileShader( m_id );

		GLint status;
		glGetShaderiv( m_id, GL_COMPILE_STATUS, &status );

		if( status == GL_FALSE )
		{
			GLint infoLogLength;
			glGetShaderiv( m_id, GL_INFO_LOG_LENGTH, &infoLogLength );

			char* strInfoLog = new char[ infoLogLength + 1 ];
			glGetShaderInfoLog( m_id, infoLogLength, NULL, strInfoLog );

			msg += strInfoLog;
			delete[] strInfoLog;
		}

		return msg;
	}

	size_t SkipWhitespace( const std::string& str, size_t start )
	{
		return str.find_first_not_of( "\r\n\t ", start );
	}

	size_t EndOfWord( const std::string& str, size_t start )
	{
		return str.find_first_of( "\r\n\t .,;[]()", start );
	}

	std::string BlockContents( const std::string& str, size_t& offset )
	{
		size_t block_start = str.find( '{', offset );
		offset = str.find( '}', block_start );
		return str.substr( block_start + 1, offset - block_start - 1 );
	}

	std::string NextWord( const std::string& str, size_t& offset )
	{
		size_t start = SkipWhitespace( str, offset );
		offset = EndOfWord( str, start );

		return str.substr( start, offset - start );
	}

	size_t NextLine( const std::string& str, size_t start )
	{
		size_t offset = str.find( "\n", start );
		return SkipWhitespace( str, offset );
	}

	std::string ShaderPart::ProcessIncludes( std::string path, std::string src )
	{
		const char* const INCLUDE = "#include \"";

		std::string base_path = path.substr( 0, path.rfind( "\\" ) );

		std::string final_source = src;

		size_t include_start = 0;
		while( (include_start = final_source.find( INCLUDE, include_start )) != std::string::npos )
		{
			size_t file_start = include_start + strlen( INCLUDE );
			size_t file_end = final_source.find( "\"", file_start );

			DD_ASSERT( file_end != std::string::npos, "'%s': Failed to find include name." );

			std::string include_name = final_source.substr( file_start, file_end - file_start );
			std::string include_path = base_path + "\\" + include_name;

			dd::File file( include_path );
			std::string include_source;
			if( !file.Read( include_source ) )
			{
				DD_ASSERT_ERROR( false, "'%s': Failed to load include path '%s'.", path.c_str(), include_path.c_str() );
			}

			include_source = ProcessIncludes( path, include_source );

			std::string source_start = final_source.substr( 0, include_start );
			std::string source_rest = final_source.substr( file_end + 1 );

			final_source.reserve( final_source.size() + include_source.size() );
			final_source = source_start + include_source + source_rest;

			include_start += include_source.size();
		}

		return final_source;
	}

	void ShaderPart::GatherUniforms( const std::string& src, UniformStorage& outUniforms )
	{
		const char* const UNIFORM = "uniform ";

		size_t uniform_start = 0;
		while( (uniform_start = src.find( UNIFORM, uniform_start )) != std::string::npos )
		{
			size_t head = uniform_start + strlen( UNIFORM );
			std::string type_name = NextWord( src, head );
			if( type_name == "struct" )
			{
				std::string struct_contents = BlockContents( src, head );

				std::string struct_name = NextWord( src, head );

				size_t index_start = src.find( '[', head );
				size_t line_end = src.find( ';', head );

				if( index_start < line_end )
				{
					size_t index_end = index_start;
					std::string index = NextWord( src, index_end );
				}

				size_t struct_head = 0;
				struct_head = SkipWhitespace( struct_contents, struct_head );
				while( struct_head != std::string::npos )
				{
					std::string struct_var_type = NextWord( struct_contents, struct_head );

					UniformType type = GetUniformTypeFromName( struct_var_type );
					DD_ASSERT( type != UniformType::Invalid );

					std::string struct_var_name = NextWord( struct_contents, struct_head );

					outUniforms.Create( struct_var_name.c_str(), type );

					struct_head = NextLine( struct_contents, struct_head );
				}
			}
			else
			{
				UniformType type = GetUniformTypeFromName( type_name );
				DD_ASSERT( type != UniformType::Invalid );

				std::string var_name = NextWord( src, head );

				outUniforms.Create( var_name.c_str(), type );
			}

			uniform_start = head;
		}
	}

	bool ShaderPart::LoadFile( const std::string& path, std::string& outSource )
	{
		dd::File file( path );
		std::string read;
		if( !file.Read( read ) )
		{
			return false;
		}

		outSource = ProcessIncludes( path, read );
		return true;
	}

	ShaderPart* ShaderPart::Create( const std::string& path, ShaderPart::Type type )
	{
		DD_PROFILE_SCOPED( Shader_Create );

		auto it = sm_shaderCache.find( path );
		if( it != sm_shaderCache.end() )
		{
			return it->second;
		}

		if( !dd::File::Exists( path ) )
		{
			return nullptr;
		}

		ShaderPart* shader = new ShaderPart( path, type );
		if( shader->m_id == OpenGL::InvalidID ||
			!shader->Reload() )
		{
			DD_ASSERT( false, "Failed to load shader from '%s'!", path.c_str() );
			delete shader;
			return nullptr;
		}

		sm_shaderCache.insert( std::make_pair( path, shader ) );
		return shader;
	}
}