//
// Shader.cpp - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PrecompiledHeader.h"
#include "Shader.h"

#include "GL/gl3w.h"

const char* s_vertex = "#version 330 core\n"
""
"layout( location = 0 ) in vec3 Position;\n"
"layout( location = 1 ) in vec3 Normal;\n"
""
"out vec3 FragmentPosition;\n"
"out vec2 FragmentUV;\n"
"flat out vec3 FragmentNormal;\n"
"out vec4 FragmentColour;\n"
""
"uniform vec4 ObjectColour;\n"
"uniform mat4 Model;\n"
"uniform mat3 NormalMatrix;\n"
"uniform mat4 View;\n"
"uniform mat4 Projection;\n"
""
"void main()"
"{"
"	FragmentPosition = vec3( Model * vec4( Position, 1 ) );\n"
"	FragmentNormal = NormalMatrix * Normal;\n"
"	//FragmentNormal = Normal;\n"
"	FragmentColour = ObjectColour;\n"
""
"	gl_Position = Projection * View * Model * vec4( Position, 1 );\n"
"}";

const char* s_pixel = "#version 330 core\n"
"out vec4 color;\n"
""
"in vec3 FragmentPosition;\n"
"flat in vec3 FragmentNormal;\n"
"in vec4 FragmentColour;\n"
""
"uniform vec3 ViewPosition;\n"

"uniform vec3 LightPosition;\n"
"uniform vec3 LightColour;\n"

"uniform float LightIntensity;\n"
"uniform float AmbientStrength;\n"
"uniform float SpecularStrength;\n"
""
"void main()"
"{"
"	vec3 lightColour = LightColour * LightIntensity;\n"
"	vec3 ambientColour = AmbientStrength * lightColour;\n"
""
"	vec3 normal = normalize( FragmentNormal );"
"	vec3 lightDir = normalize( LightPosition - FragmentPosition );\n"
"	float diffAmount = max( dot( normal, lightDir ), 0.0 );\n"
"	vec3 diffuseColour = diffAmount * lightColour;\n"
""
"	vec3 viewDir = normalize( ViewPosition - FragmentPosition );\n"
"	vec3 reflectDir = reflect( -lightDir, normal );\n"
"	float specularAmount = pow( max( dot( viewDir, reflectDir ), 0.0 ), 32 );\n"
"	vec3 specularColour = SpecularStrength * specularAmount * lightColour;\n"
""
"	color.rgb = (specularColour + diffuseColour + ambientColour) * FragmentColour.rgb;\n"
"	//color.rgb = (vec3(1,1,1) + FragmentNormal) * 0.5;\n"
"	color.a = FragmentColour.a;\n"
"}";

namespace dd
{
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

	Shader Shader::Create( const String& name, const String& path, Shader::Type type )
	{
		DD_PROFILE_SCOPED( Shader_Create );

		String256 source;

		// TODO: Load source from file.
		if( type == Type::Vertex )
		{
			source = s_vertex;
		}

		if( type == Type::Pixel )
		{
			source = s_pixel;
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