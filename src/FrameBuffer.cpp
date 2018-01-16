//
// FrameBuffer.cpp - A wrapper around OpenGL frame buffers.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#include "PrecompiledHeader.h"
#include "FrameBuffer.h"

#include "GLError.h"
#include "ICamera.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Window.h"

#include "GL/gl3w.h"

namespace dd
{
	VAO FrameBuffer::m_fullScreenVAO;
	VBO FrameBuffer::m_fullScreenVBO;
	ShaderHandle FrameBuffer::m_blitShader;

	static const glm::vec3 s_fullScreenQuadVertices[] = {
		glm::vec3( -1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f,  1.0f, 0.0f )
	};

	static const ConstBuffer<glm::vec3> s_fullScreenQuadBuffer( s_fullScreenQuadVertices, 6 );
	
	FrameBuffer::FrameBuffer()
	{

	}

	FrameBuffer::FrameBuffer( const FrameBuffer& other )
	{
		m_valid = other.m_valid;
		m_fbo = other.m_fbo;
		m_colour = other.m_colour;
		m_depth = other.m_depth;
	}

	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}

	FrameBuffer& FrameBuffer::operator=( const FrameBuffer& other )
	{
		m_valid = other.m_valid;
		m_fbo = other.m_fbo;
		m_colour = other.m_colour;
		m_depth = other.m_depth;

		return *this;
	}

	bool FrameBuffer::Create( Texture& target, Texture* depth )
	{
		m_colour = &target;

		glGenFramebuffers( 1, &m_fbo );
		CheckGLError();

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
		CheckGLError();

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colour->ID(), 0 );
		CheckGLError();

		GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers( m_fbo, 1, draw_buffers );
		CheckGLError();

		if( depth != nullptr )
		{
			m_depth = depth;

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->ID(), 0 );
			CheckGLError();
		}

		GLuint status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
		{
			DD_ASSERT( false, "Failed to create framebuffer!" );
			return false;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		m_valid = true;
		return true;
	}

	void FrameBuffer::RenderInit()
	{
		if( !m_blitShader.IsValid() )
		{
			Vector<Shader*> shaders;
			shaders.Add( Shader::Create( String32( "shaders\\blit.vertex" ), Shader::Type::Vertex ) );
			shaders.Add( Shader::Create( String32( "shaders\\blit.pixel" ), Shader::Type::Pixel ) );
			
			m_blitShader = ShaderProgram::Create( String8( "blit" ), shaders );
			ShaderProgram& shader = *m_blitShader.Get();

			shader.Use( true );

			shader.SetPositionsName( "Position" );
			
			m_fullScreenVAO.Create();
			m_fullScreenVAO.Bind();

			m_fullScreenVBO.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
			m_fullScreenVBO.Bind();
			m_fullScreenVBO.SetData( s_fullScreenQuadBuffer );

			shader.BindPositions();

			m_fullScreenVAO.Unbind();

			shader.Use( false );
		}
	}

	void FrameBuffer::RenderDepth()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_depth != nullptr );

		ShaderProgram& shader = *m_blitShader.Get();
		shader.Use( true );

		m_depth->Bind( 0 );

		shader.SetUniform( "Texture", *m_depth );
		shader.SetUniform( "DrawDepth", true );
		
		m_fullScreenVAO.Bind();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckGLError();

		m_fullScreenVAO.Unbind();

		m_depth->Unbind();

		shader.Use( false );
	}

	void FrameBuffer::Render()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_colour != nullptr );

		ShaderProgram& shader = *m_blitShader.Get();
		shader.Use( true );

		m_fullScreenVAO.Bind();
		
		m_colour->Bind( 0 );
		
		shader.SetUniform( "Texture", *m_colour );
		shader.SetUniform( "DrawDepth", false );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckGLError();

		m_colour->Unbind();

		m_fullScreenVAO.Unbind();

		shader.Use( false );
	}

	void FrameBuffer::Blit()
	{
		GLint viewport[ 4 ];
		glGetIntegerv( GL_VIEWPORT, viewport );

		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_previousRead );
		CheckGLError();

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo );
		CheckGLError();

		glBlitFramebuffer(
			0, 0, m_colour->GetSize().x, m_colour->GetSize().y,
			0, 0, viewport[2], viewport[3],
			GL_COLOR_BUFFER_BIT, GL_LINEAR );
		CheckGLError();

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		CheckGLError();
	}

	void FrameBuffer::Destroy()
	{
		if( m_fbo != OpenGL::InvalidID )
		{
			glDeleteBuffers( 1, &m_fbo );
			CheckGLError();

			m_fbo = OpenGL::InvalidID;
		}

		m_depth = nullptr;
		m_colour = nullptr;
		m_valid = false;
	}

	void FrameBuffer::Bind()
	{
		DD_ASSERT( IsValid() );

		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_previousRead );
		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &m_previousDraw );

		GLint viewport[ 4 ];
		glGetIntegerv( GL_VIEWPORT, viewport );
		m_previousSize = glm::vec2( viewport[ 2 ], viewport[ 3 ] );
		CheckGLError();

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
		glViewport( 0, 0, m_colour->GetSize().x, m_colour->GetSize().y );
		CheckGLError();

		glClearDepthf( m_clearDepth );
		glClearColor( m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		CheckGLError();
	}
	
	void FrameBuffer::Unbind()
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_previousDraw );
		CheckGLError();

		glViewport( 0, 0, m_previousSize.x, m_previousSize.y );
		CheckGLError();
	}
}