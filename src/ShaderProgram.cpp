//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PrecompiledHeader.h"
#include "ShaderProgram.h"

#include "GLError.h"
#include "Shader.h"
#include "ShaderHandle.h"
#include "Texture.h"

#include "GL/gl3w.h"

#include "glm/gtc/type_ptr.hpp"

namespace dd
{
	std::unordered_map<uint64, ShaderProgram> ShaderProgram::s_instances;

	ShaderHandle ShaderProgram::Create( const String& name, const Vector<Shader*>& shaders )
	{
		DD_PROFILE_SCOPED( ShaderProgram_Create );

		uint64 hash = dd::Hash( name );

		auto it = s_instances.find( hash );
		if( it == s_instances.end() )
		{
			ShaderProgram program = CreateInstance( name, shaders );
			if( program.IsValid() )
			{
				s_instances.insert( std::make_pair( hash, program ) );
			}
			else
			{
				hash = 0;

				DD_ASSERT_ERROR( false, "ShaderProgram::CreateInstance returned an invalid program!" );
			}
		}
		else
		{
			__debugbreak();
		}

		ShaderHandle handle;
		handle.m_hash = hash;

		return handle;
	}

	ShaderProgram* ShaderProgram::Get( ShaderHandle handle )
	{
		auto it = s_instances.find( handle.m_hash );
		if( it == s_instances.end() )
		{
			return nullptr;
		}

		return &it->second;
	}

	void ShaderProgram::Destroy( ShaderHandle handle )
	{
		s_instances.erase( handle.m_hash );
	}

	ShaderProgram::ShaderProgram( const String& name )
		: m_name( name ),
		m_refCount( nullptr ),
		m_inUse( false )
	{
		m_id = glCreateProgram();
		CheckGLError();

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "glCreateProgram failed!" );

		m_refCount = new std::atomic<int>( 1 );
	}

	ShaderProgram::ShaderProgram( const ShaderProgram& other ) :
		m_id( other.m_id ),
		m_valid( other.m_valid ),
		m_name( other.m_name ),
		m_refCount( other.m_refCount ),
		m_inUse( other.m_inUse ),
		m_positionsName( m_positionsName ),
		m_normalsName( m_normalsName ),
		m_uvsName( m_uvsName ),
		m_vertexColoursName( m_vertexColoursName ),
		m_shaders( other.m_shaders )
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

		m_valid = other.m_valid;
		m_id = other.m_id;
		m_name = other.m_name;
		m_refCount = other.m_refCount;
		m_inUse = other.m_inUse;

		m_positionsName = m_positionsName;
		m_normalsName = m_normalsName;
		m_uvsName = m_uvsName;
		m_vertexColoursName = m_vertexColoursName;

		m_shaders = other.m_shaders;

		Retain();

		return *this;
	}

	ShaderProgram ShaderProgram::CreateInstance( const String& name, const Vector<Shader*>& shaders )
	{
		ShaderProgram program( name );
		program.m_valid = true;

		if( shaders.Size() == 0 )
		{
			DD_ASSERT_ERROR( shaders.Size() > 0, "Failed to provide any shaders to ShaderProgram!" );

			program.m_valid = false;
		}

		for( const Shader* shader : shaders )
		{
			DD_ASSERT_ERROR( shader != nullptr, "Invalid shader given to program!" );

			glAttachShader( program.m_id, shader->m_id );
			CheckGLError();
		}

		String256 msg = program.Link();
		if( !msg.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "Linking program failed: %s", msg.c_str() );

			program.m_valid = false;
		}

		program.m_shaders = shaders;
		return program;
	}

	void ShaderProgram::ReloadAllShaders()
	{
		for( auto it : s_instances )
		{
			it.second.Reload();
		}
	}

	String256 ShaderProgram::Link()
	{
		DD_PROFILE_SCOPED( ShaderProgram_Link );

		glLinkProgram( m_id );

		String256 msg;

		GLint status;
		glGetProgramiv( m_id, GL_LINK_STATUS, &status );
		CheckGLError();

		if( status == GL_FALSE )
		{
			msg = "Program linking failure: ";

			GLint infoLogLength;
			glGetProgramiv( m_id, GL_INFO_LOG_LENGTH, &infoLogLength );
			CheckGLError();

			char* strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog( m_id, infoLogLength, NULL, strInfoLog );
			CheckGLError();

			strInfoLog[ infoLogLength ] = '\0';

			msg += strInfoLog;
			delete[] strInfoLog;

			m_valid = false;
		}

		return msg;
	}

	bool ShaderProgram::Reload()
	{
		for( Shader* shader : m_shaders )
		{
			if( !shader->Reload() )
			{
				break;
			}
		}

		if( InUse() )
		{
			Use( false );
		}

		String256 msg = Link();
		if( !msg.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "Linking program failed!\n%s", msg.c_str() );
			return false;
		}

		Use( true );

		BindPositions();
		BindNormals();
		BindUVs();
		BindVertexColours();

		Use( false );

		return true;
	}

	bool ShaderProgram::InUse() const
	{
		return m_inUse;
	}

	void ShaderProgram::Use( bool use )
	{
		DD_ASSERT( use != m_inUse, "Trying to use shader that is already in use!" );

		int current;
		glGetIntegerv( GL_CURRENT_PROGRAM, &current );
		CheckGLError();

		if( use )
		{
			DD_ASSERT( current == 0, "Shader not deactivated before trying to activate another shader!" );
		}
		else
		{
			DD_ASSERT( current == m_id, "Trying to deactivate a different shader than currently bound!" );
		}

		m_inUse = use;

		glUseProgram( m_inUse ? m_id : 0 );
		CheckGLError();
	}

	ShaderLocation ShaderProgram::GetAttribute( const char* name ) const
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty attribute name given!" );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name );

		CheckGLError();

		return attrib;
	}

	void ShaderProgram::DisableAttribute( const char* name )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty attribute name given!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glDisableVertexAttribArray( loc );
			CheckGLError();
		}
	}

	void ShaderProgram::EnableAttribute( const char* name )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty attribute name given!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glEnableVertexAttribArray( loc );
			CheckGLError();
		}
	}

	void ShaderProgram::SetPositionsName( const char* name )
	{
		m_positionsName = name;
	}

	bool ShaderProgram::BindPositions()
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );

		if( m_positionsName.IsEmpty() )
		{
			return false;
		}

		return BindAttributeFloat( m_positionsName.c_str(), 3, 0, 0, false );
	}

	void ShaderProgram::SetNormalsName( const char* name )
	{
		m_normalsName = name;
	}

	bool ShaderProgram::BindNormals()
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );

		if( m_normalsName.IsEmpty() )
		{
			return false;
		}
		
		return BindAttributeFloat( m_normalsName.c_str(), 3, 0, 0, true );
	}

	void ShaderProgram::SetUVsName( const char* name )
	{
		m_uvsName = name;
	}

	bool ShaderProgram::BindUVs()
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		
		if( m_uvsName.IsEmpty() )
		{
			return false;
		}

		return BindAttributeFloat( m_uvsName.c_str(), 2, 0, 0, false );
	}

	void ShaderProgram::SetVertexColoursName( const char* name )
	{
		m_vertexColoursName = name;
	}

	bool ShaderProgram::BindVertexColours()
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );

		if( m_vertexColoursName.IsEmpty() )
		{
			return false;
		}

		return BindAttributeFloat( m_vertexColoursName.c_str(), 4, 0, 0, false );
	}

	bool ShaderProgram::BindAttributeFloat( const char* name, uint components, uint stride, uint first, bool normalized )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glVertexAttribPointer( loc, components, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, stride * sizeof( float ), (const GLvoid*) (uint64) (first * sizeof( float )) );
			CheckGLError();

			glEnableVertexAttribArray( loc );
			CheckGLError();

			return true;
		}

		return false;
	}

	ShaderLocation ShaderProgram::GetUniform( const char* name ) const
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );
		return uniform;
	}

	void ShaderProgram::SetUniform( const char* name, float f )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1f( uniform, f );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, int i )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, i );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, bool b )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, b );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec3& vec )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform3fv( uniform, 1, glm::value_ptr( vec ) );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec4& vec )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform4fv( uniform, 1, glm::value_ptr( vec ) );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::mat3& mat )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix3fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::mat4& mat )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix4fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const Texture& texture )
	{
		DD_ASSERT( m_inUse, "Need to use shader before trying to access it!" );
		DD_ASSERT( IsValid(), "Program is invalid!" );
		DD_ASSERT( strlen( name ) > 0, "Empty uniform name given!" );

		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, texture.GetTextureUnit() );
			CheckGLError();
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
			for( const Shader* shader : m_shaders )
			{
				glDetachShader( m_id, shader->m_id );
			}

			if( m_id != OpenGL::InvalidID )
			{
				glDeleteProgram( m_id );
				m_id = OpenGL::InvalidID;
			}

			delete m_refCount;
			m_refCount = nullptr;
		}
	}
}