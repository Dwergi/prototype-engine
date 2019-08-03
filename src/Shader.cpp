//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PCH.h"
#include "Shader.h"

#include "GLError.h"
#include "ICamera.h"
#include "OpenGL.h"
#include "ShaderPart.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"

DD_POD_CPP(ddr::ShaderHandle);

namespace ddr
{
	ScopedShader::ScopedShader( Shader& shader ) :
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

	Shader::Shader()
	{
		m_id = glCreateProgram();
		CheckOGLError();

		DD_ASSERT_ERROR( m_id != OpenGL::InvalidID, "glCreateProgram failed!" );
	}

	Shader::~Shader()
	{
		for( const ShaderPart* shader : m_shaders )
		{
			glDetachShader( m_id, shader->m_id );
		}

		if( m_id != OpenGL::InvalidID )
		{
			glDeleteProgram( m_id );
			m_id = OpenGL::InvalidID;
		}
	}

	void Shader::SetShaders( const dd::Vector<ShaderPart*>& shaders )
	{
		m_valid = true;

		if( shaders.Size() == 0 )
		{
			DD_ASSERT_ERROR( shaders.Size() > 0, "ShaderProgram '%s': Failed to provide any shaders!", m_name.c_str() );

			m_valid = false;
		}

		for( const ShaderPart* shader : shaders )
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

	dd::String256 Shader::Link()
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

	bool Shader::Reload()
	{
		for( ShaderPart* shader : m_shaders )
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

	bool Shader::InUse() const
	{
		return m_inUse;
	}

	void Shader::Use( bool use )
	{
		DD_ASSERT( use != m_inUse, "ShaderProgram '%s': Trying to use shader that is already in use!", m_name.c_str() );

		dd::Profiler::ShaderChanged();

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

	ScopedShader Shader::UseScoped()
	{
		return ScopedShader( *this );
	}

	ShaderLocation Shader::GetAttribute( std::string name ) const
	{
		AssertBeforeUse( name );

		GLint attrib = glGetAttribLocation( m_id, (const GLchar*) name.c_str() );
		CheckOGLError();

		return attrib;
	}

	bool Shader::EnableAttribute( std::string name )
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

	bool Shader::DisableAttribute( std::string name )
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

	bool Shader::BindPositions()
	{
		return BindAttributeVec3( "Position", false );
	}

	bool Shader::BindNormals()
	{
		return BindAttributeVec3( "Normal", true );
	}

	bool Shader::BindUVs()
	{
		return BindAttributeVec2( "UV", false );
	}

	bool Shader::BindVertexColours()
	{
		return BindAttributeVec4( "VertexColour", false );
	}

	bool Shader::BindAttributeVec2( std::string name, bool normalized )
	{
		return BindAttributeFloat( name, 2, normalized );
	}

	bool Shader::BindAttributeVec3( std::string name, bool normalized )
	{
		return BindAttributeFloat( name, 3, normalized );
	}

	bool Shader::BindAttributeVec4( std::string name, bool normalized )
	{
		return BindAttributeFloat( name, 4, normalized );
	}

	bool Shader::BindAttributeFloat( std::string name, uint components, bool normalized )
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

	bool Shader::SetAttributeInstanced( std::string name )
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

	ShaderLocation Shader::GetUniform( std::string name ) const
	{
		AssertBeforeUse( name );

		GLint uniform = glGetUniformLocation( m_id, (const GLchar*) name.c_str() );
		return uniform;
	}

	void Shader::SetUniform( std::string name, float f )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1f( uniform, f );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, int i )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, i );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, bool b )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, b );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const glm::vec2& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform2fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const glm::vec3& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform3fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const glm::vec4& vec )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform4fv( uniform, 1, glm::value_ptr( vec ) );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const glm::mat3& mat )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix3fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const glm::mat4& mat )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniformMatrix4fv( uniform, 1, false, glm::value_ptr( mat ) );
			CheckOGLError();
		}
	}

	void Shader::SetUniform( std::string name, const Texture& texture )
	{
		ShaderLocation uniform = GetUniform( name );
		if( uniform != InvalidLocation )
		{
			glUniform1i( uniform, texture.GetTextureUnit() );
			CheckOGLError();
		}
	}

	void Shader::AssertBeforeUse( std::string name ) const
	{
		DD_ASSERT( InUse(), "ShaderProgram '%s': Need to use shader before trying to access it!", m_name.c_str() );
		DD_ASSERT( IsValid(), "ShaderProgram '%s': Program is invalid!", m_name.c_str() );
		DD_ASSERT( name.size() > 0, "ShaderProgram '%s': Empty uniform name given!", m_name.c_str() );
	}

	ShaderHandle ShaderManager::Load( std::string name )
	{
		ShaderHandle shader_h = super::Find( name );
		if( !shader_h.IsValid() )
		{
			const std::string folder( "shaders\\" );

			dd::Vector<ShaderPart*> shaders;
			shaders.Reserve( 3 );

			{
				std::string vertex_path = folder + name + ".vert";

				ShaderPart* vertex = ShaderPart::Create( vertex_path, ShaderPart::Type::Vertex );
				DD_ASSERT( vertex != nullptr );
				shaders.Add( vertex );
			}

			{
				std::string geometry_path = folder + name + ".geom";

				ShaderPart* geom = ShaderPart::Create( geometry_path, ShaderPart::Type::Geometry );
				if( geom != nullptr )
				{
					shaders.Add( geom );
				}
			}

			{
				std::string pixel_path = folder + name + ".frag";

				ShaderPart* pixel = ShaderPart::Create( pixel_path, ShaderPart::Type::Pixel );
				DD_ASSERT( pixel != nullptr );
				shaders.Add( pixel );
			}

			shader_h = super::Create( name );

			Shader* program = shader_h.Access();
			program->SetShaders( shaders );
		}

		return shader_h;
	}

	void ShaderManager::ReloadAll()
	{
		for( size_t i = 0; i < super::LiveCount(); ++i )
		{
			Shader* program = super::AccessNth( i );
			program->Reload();
		}
	}

}