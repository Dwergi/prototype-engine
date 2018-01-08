//
// RenderToTexture.cpp - A wrapper around OpenGL frame buffers.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#include "PrecompiledHeader.h"
#include "RenderToTexture.h"

#include "GLError.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Window.h"

#include "GL/gl3w.h"

namespace dd
{
	VAO RenderToTexture::m_fullScreenVAO;
	VBO RenderToTexture::m_fullScreenVBO;
	ShaderHandle RenderToTexture::m_blitShader;

	static const glm::vec3 s_fullScreenQuadVertices[] = {
		glm::vec3( -1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f,  1.0f, 0.0f )
	};

	static const ConstBuffer<glm::vec3> s_fullScreenQuadBuffer( s_fullScreenQuadVertices, 6 );
	
	RenderToTexture::RenderToTexture( const Window& window ) : 
		m_window( &window )
	{

	}

	RenderToTexture::RenderToTexture( const RenderToTexture& other ) :
		m_window( other.m_window )
	{
		m_valid = other.m_valid;
		m_frameBuffer = other.m_frameBuffer;
		m_texture = other.m_texture;
		m_depth = other.m_depth;
	}

	RenderToTexture::~RenderToTexture()
	{
		Destroy();
	}

	RenderToTexture& RenderToTexture::operator=( const RenderToTexture& other )
	{
		m_window = other.m_window;
		m_valid = other.m_valid;
		m_frameBuffer = other.m_frameBuffer;
		m_texture = other.m_texture;
		m_depth = other.m_depth;

		return *this;
	}

	bool RenderToTexture::Create( Texture& target, bool depth )
	{
		m_texture = &target;

		glGenFramebuffers( 1, &m_frameBuffer );
		CheckGLError();

		glBindFramebuffer( GL_FRAMEBUFFER, m_frameBuffer );
		CheckGLError();

		if( depth )
		{
			glGenRenderbuffers( 1, &m_depth );
			glBindRenderbuffer( GL_RENDERBUFFER, m_depth );
			glNamedRenderbufferStorage( m_depth, GL_DEPTH_COMPONENT, m_texture->GetSize().x, m_texture->GetSize().y );
			glNamedFramebufferRenderbuffer( m_frameBuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth );
		}

		glNamedFramebufferTexture( m_frameBuffer, GL_COLOR_ATTACHMENT0, m_texture->ID(), 0 );

		GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers( m_frameBuffer, 1, draw_buffers );

		GLuint status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
			return false;

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		m_valid = true;
		return true;
	}

	void RenderToTexture::PreRender()
	{
		if( !m_blitShader.IsValid() )
		{
			Vector<Shader> shaders;
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

	void RenderToTexture::Render()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_texture != nullptr );

		ShaderProgram& shader = *m_blitShader.Get();
		shader.Use( true );

		m_fullScreenVAO.Bind();

		m_texture->Bind( 0 );

		shader.SetUniform( "Texture", *m_texture );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckGLError();

		m_texture->Unbind();

		m_fullScreenVAO.Unbind();

		shader.Use( false );
	}

	void RenderToTexture::Destroy()
	{
		if( m_frameBuffer != OpenGL::InvalidID )
		{
			glDeleteBuffers( 1, &m_frameBuffer );
			CheckGLError();

			m_frameBuffer = OpenGL::InvalidID;
		}

		if( m_depth != OpenGL::InvalidID )
		{
			glDeleteBuffers( 1, &m_depth );
			CheckGLError();

			m_depth = OpenGL::InvalidID;
		}

		m_texture = nullptr;
		m_valid = false;
	}

	void RenderToTexture::Bind()
	{
		DD_ASSERT( IsValid() );

		glBindFramebuffer( GL_FRAMEBUFFER, m_frameBuffer );
		glViewport( 0, 0, m_texture->GetSize().x, m_texture->GetSize().y );
		CheckGLError();

		glClearColor( m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		CheckGLError();
	}
	
	void RenderToTexture::Unbind()
	{
		// TODO: Should use glGet for the resolution in Bind() and restore that
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glViewport( 0, 0, m_window->GetWidth(), m_window->GetHeight() );
		CheckGLError();
	}
}