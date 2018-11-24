//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PCH.h"
#include "ShaderProgram.h"

#include "GLError.h"
#include "ICamera.h"
#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"

DD_HANDLE_MANAGER( ddr::ShaderProgram );

namespace ddr
{
	ScopedShader::ScopedShader( ShaderProgram& shader ) :
		m_shader( &shader )
	{
		m_shader->Use( true );
	}

	ScopedShader::~ScopedShader()
	{
		if( m_shader != nullptr )
		{
			m_shader->Use( false );
		}
	}

	ShaderProgram::ShaderProgram()
	{
		m_id = glCreateProgram();
		CheckOGLError();

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "glCreateProgram failed!" );
	}

	ShaderProgram::~ShaderProgram()
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
	}

	void ShaderProgram::SetShaders( const dd::Vector<Shader*>& shaders )
	{
		m_valid = true;

		if( shaders.Size() == 0 )
		{
			DD_ASSERT_ERROR( shaders.Size() > 0, "ShaderProgram '%s': Failed to provide any shaders!", m_name.c_str() );

			m_valid = false;
		}

		for( const Shader* shader : shaders )
		{
			DD_ASSERT_ERROR( shader != nullptr, "ShaderProgram '%s': Invalid shader given to program!", m_name.c_str() );

			glAttachShader( m_id, shader->m_id );
			CheckOGLError();
		}

		dd::String256 msg = Link();
		if( !msg.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "ShaderProgram '%s': %s", m_name.c_str(), msg.c_str() );

			m_valid = false;
		}

		m_shaders = shaders;
	}

	dd::String256 ShaderProgram::Link()
	{
		DD_PROFILE_SCOPED( ShaderProgram_Link );

		glLinkProgram( m_id );

		dd::String256 msg;

		GLint status;
		glGetProgramiv( m_id, GL_LINK_STATUS, &status );
		CheckOGLError();

		if( status == GL_FALSE )
		{
			msg = "\nProgram linking failure: \n";

			GLint infoLogLength;
			glGetProgramiv( m_id, GL_INFO_LOG_LENGTH, &infoLogLength );
			CheckOGLError();

			char* strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog( m_id, infoLogLength, NULL, strInfoLog );
			CheckOGLError();

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

		dd::String256 msg = Link();
		if( !msg.IsEmpty() )
		{
			DD_ASSERT_ERROR( false, "ShaderProgram '%s': %s", m_name.c_str(), msg.c_str() );
			return false;
		}

		return true;
	}

	bool ShaderProgram::InUse() const
	{
		return m_inUse;
	}

	void ShaderProgram::Use( bool use )
	{
		DD_ASSERT( use != m_inUse, "ShaderProgram '%s': Trying to use shader that is already in use!", m_name.c_str() );

		int current;
		glGetIntegerv( GL_CURRENT_PROGRAM, &current );
		CheckOGLError();

		if( use )
		{
			DD_ASSERT( current == 0, "ShaderProgram '%s': Shader not deactivated before trying to activate another shader!", m_name.c_str() );
		}
		else
		{
			DD_ASSERT( current == m_id, "ShaderProgram '%s': Trying to deactivate a different shader than currently bound!", m_name.c_str() );
		}

		m_inUse = use;

		glUseProgram( m_inUse ? m_id : 0 );
		CheckOGLError();
	}

	ScopedShader ShaderProgram::UseScoped()
	{
		return ScopedShader( *this );
	}

	ShaderLocation ShaderProgram::GetAttribute( const char* name ) const
	{
		AssertBeforeUse( name );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name );
		CheckOGLError();

		return attrib;
	}

	bool ShaderProgram::EnableAttribute( const char* name )
	{
		GLint currentVAO = VAO::GetCurrentVAO();
		DD_ASSERT( currentVAO != OpenGL::InvalidID, "No VAO is bound!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glEnableVertexAttribArray( loc );
			CheckOGLError();

			return true;
		}

		DD_ASSERT( false, "Attribute '%s' not found!", name );
		return false;
	}

	bool ShaderProgram::DisableAttribute( const char* name )
	{
		GLint currentVAO = VAO::GetCurrentVAO();
		DD_ASSERT( currentVAO != OpenGL::InvalidID, "No VAO is bound!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glDisableVertexAttribArray( loc );
			CheckOGLError();

			return true;
		}

		DD_ASSERT( false, "Attribute '%s' not found!", name );
		return false;
	}

	bool ShaderProgram::BindPositions()
	{
		return BindAttributeVec3( "Position", false );
	}

	bool ShaderProgram::BindNormals()
	{
		return BindAttributeVec3( "Normal", true );
	}

	bool ShaderProgram::BindUVs()
	{
		return BindAttributeVec2( "UV", false );
	}

	bool ShaderProgram::BindVertexColours()
	{
		return BindAttributeVec4( "VertexColour", false );
	}

	bool ShaderProgram::BindAttributeVec2( const char* name, bool normalized )
	{
		return BindAttributeFloat( name, 2, normalized );
	}

	bool ShaderProgram::BindAttributeVec3( const char* name, bool normalized )
	{
		return BindAttributeFloat( name, 3, normalized );
	}

	bool ShaderProgram::BindAttributeVec4( const char* name, bool normalized )
	{
		return BindAttributeFloat( name, 4, normalized );
	}

	bool ShaderProgram::BindAttributeFloat( const char* name, uint components, bool normalized )
	{
		GLint currentVAO = VAO::GetCurrentVAO();
		DD_ASSERT( currentVAO != OpenGL::InvalidID, "No VAO is bound!" );

		GLint currentVBO = VBO::GetCurrentVBO( GL_ARRAY_BUFFER );
		DD_ASSERT( currentVBO != OpenGL::InvalidID, "No VBO is bound!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glVertexAttribPointer( loc, components, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, 0, nullptr );
			CheckOGLError();

			glEnableVertexAttribArray( loc );
			CheckOGLError();

			return true;
		}

		return false;
	}

	bool ShaderProgram::SetAttributeInstanced( const char* name )
	{
		GLint currentVAO = VAO::GetCurrentVAO();
		DD_ASSERT( currentVAO != OpenGL::InvalidID, "No VAO is bound!" );

		ShaderLocation loc = GetAttribute( name );
		if( loc != InvalidLocation )
		{
			glVertexAttribDivisor( loc, 1 );
			CheckOGLError();

			return true;
		}

		DD_ASSERT( false, "Attribute '%s' not found!", name );
		return false;
	}

	ShaderLocation ShaderProgram::GetUniform( const char* name ) const
	{
		AssertBeforeUse( name );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name );
		return uniform;
	}

	void ShaderProgram::SetUniform( const char* name, float f )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1f( uniform, f );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, int i )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, i );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, bool b )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, b );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec2& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform2fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec3& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform3fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::vec4& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform4fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::mat3& mat )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix3fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const glm::mat4& mat )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix4fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckOGLError();
		}
	}

	void ShaderProgram::SetUniform( const char* name, const Texture& texture )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, texture.GetTextureUnit() );
			CheckOGLError();
		}
	}

	void ShaderProgram::AssertBeforeUse( const char* name ) const
	{
		DD_ASSERT( InUse(), "ShaderProgram '%s': Need to use shader before trying to access it!", m_name.c_str() );
		DD_ASSERT( IsValid(), "ShaderProgram '%s': Program is invalid!", m_name.c_str() );
		DD_ASSERT( strlen( name ) > 0, "ShaderProgram '%s': Empty uniform name given!", m_name.c_str() );
	}

	ShaderHandle ShaderManager::Load( const char* name )
	{
		ShaderHandle shader_h = base::Find( name );
		if( !shader_h.IsValid() )
		{
			const dd::String16 folder( "shaders\\" );

			dd::Vector<Shader*> shaders;
			shaders.Reserve( 3 );

			{
				dd::String32 vertexPath( folder );
				vertexPath += name;
				vertexPath += ".vertex";

				Shader* vertex = Shader::Create( vertexPath, Shader::Type::Vertex );
				DD_ASSERT( vertex != nullptr );
				shaders.Add( vertex );
			}

			{
				dd::String32 geometryPath( folder );
				geometryPath += name;
				geometryPath += ".geometry";

				Shader* geom = Shader::Create( geometryPath, Shader::Type::Geometry );
				if( geom != nullptr )
				{
					shaders.Add( geom );
				}
			}

			{
				dd::String32 pixelPath( folder );
				pixelPath += name;
				pixelPath += ".pixel";

				Shader* pixel = Shader::Create( pixelPath, Shader::Type::Pixel );
				DD_ASSERT( pixel != nullptr );
				shaders.Add( pixel );
			}

			shader_h = base::Create( name );

			ShaderProgram* program = shader_h.Access();
			program->SetShaders( shaders );
		}

		return shader_h;
	}

	void ShaderManager::ReloadAll()
	{
		for( size_t i = 0; i < base::Count(); ++i )
		{
			ShaderProgram* program = base::AccessAt( i );
			program->Reload();
		}
	}

}