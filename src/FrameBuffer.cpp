//
// FrameBuffer.cpp - A wrapper around OpenGL frame buffers.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#include "PCH.h"
#include "FrameBuffer.h"

#include "GLError.h"
#include "ICamera.h"
#include "OpenGL.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "Texture.h"
#include "Uniforms.h"
#include "IWindow.h"

namespace ddr
{
	VAO FrameBuffer::m_vaoFullscreen;
	VBO FrameBuffer::m_vboFullscreen;
	ShaderHandle FrameBuffer::m_blitShader;

	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec3 s_fullScreenQuadVertices[] = {
		glm::vec3( -1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( -1.0f,  1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f,  1.0f, 0.0f )
	};

	static const dd::ConstBuffer<glm::vec3> s_fullScreenQuadBuffer( s_fullScreenQuadVertices, 6 );
		
	FrameBuffer::FrameBuffer()
	{

	}

	FrameBuffer::FrameBuffer( const FrameBuffer& other )
	{
		m_valid = other.m_valid;
		m_fbo = other.m_fbo;
		m_texColour = other.m_texColour;
		m_texDepth = other.m_texDepth;
	}

	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}

	FrameBuffer& FrameBuffer::operator=( const FrameBuffer& other )
	{
		m_valid = other.m_valid;
		m_fbo = other.m_fbo;
		m_texColour = other.m_texColour;
		m_texDepth = other.m_texDepth;

		return *this;
	}

	bool FrameBuffer::Create( Texture& target, Texture* depth )
	{
		m_texColour = &target;

		glGenFramebuffers( 1, &m_fbo );
		CheckOGLError();

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
		CheckOGLError();

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColour->ID(), 0 );
		CheckOGLError();

		GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers( m_fbo, 1, draw_buffers );
		CheckOGLError();

		if( depth != nullptr )
		{
			m_texDepth = depth;

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texDepth->ID(), 0 );
			CheckOGLError();
		}

		GLuint status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
		{
			DD_ASSERT( status != GL_FRAMEBUFFER_COMPLETE, "Failed to create framebuffer!" );
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
			m_blitShader = s_shaderManager->Load( "blit" );

			Shader* shader = m_blitShader.Access();
			shader->Use( true );

			m_vaoFullscreen.Create();
			m_vaoFullscreen.Bind();

			m_vboFullscreen.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
			m_vboFullscreen.Bind();
			m_vboFullscreen.SetData( s_fullScreenQuadBuffer );
			m_vboFullscreen.CommitData();

			shader->BindPositions();

			m_vboFullscreen.Unbind();

			m_vaoFullscreen.Unbind();

			shader->Use( false );
		}
	}

	void FrameBuffer::RenderDepth( ddr::UniformStorage& uniforms, const ddr::ICamera& camera )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_texDepth != nullptr );

		Shader* shader = m_blitShader.Access();
		ScopedShader scoped_shader = shader->UseScoped();

		m_vaoFullscreen.Bind();
		m_texDepth->Bind( 0 );

		CheckOGLError();

		uniforms.Set( "Texture", *m_texDepth );
		uniforms.Set( "Near", camera.GetNear() );
		uniforms.Set( "DrawDepth", true );

		uniforms.Bind( *shader );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckOGLError();

		uniforms.Unbind();

		m_texDepth->Unbind();
		m_vaoFullscreen.Unbind();

		CheckOGLError();
	}

	void FrameBuffer::Render( ddr::UniformStorage& uniforms )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_texColour != nullptr );

		Shader* shader = m_blitShader.Access();
		ScopedShader scoped_shader = shader->UseScoped();

		m_vaoFullscreen.Bind();
		
		m_texColour->Bind( 0 );
		
		uniforms.Set( "Texture", *m_texColour );
		uniforms.Set( "DrawDepth", false );

		uniforms.Bind( *shader );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckOGLError();

		uniforms.Unbind();

		m_texColour->Unbind();

		m_vaoFullscreen.Unbind();
	}

	void FrameBuffer::Blit()
	{
		GLint viewport[ 4 ];
		glGetIntegerv( GL_VIEWPORT, viewport );

		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_previousRead );
		CheckOGLError();

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo );
		CheckOGLError();

		glBlitFramebuffer(
			0, 0, m_texColour->GetSize().x, m_texColour->GetSize().y,
			0, 0, viewport[2], viewport[3],
			GL_COLOR_BUFFER_BIT, GL_LINEAR );
		CheckOGLError();

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		CheckOGLError();
	}

	void FrameBuffer::Destroy()
	{
		UnbindRead();
		UnbindDraw();

		if( m_fbo != OpenGL::InvalidID )
		{
			glDeleteBuffers( 1, &m_fbo );
			CheckOGLError();

			m_fbo = OpenGL::InvalidID;
		}

		m_texDepth = nullptr;
		m_texColour = nullptr;
		m_valid = false;
	}

	void FrameBuffer::Clear()
	{
		glClearDepthf( m_clearDepth );
		glClearColor( m_clearColour.r, m_clearColour.g, m_clearColour.b, m_clearColour.a );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		CheckOGLError();
	}

	void FrameBuffer::BindRead()
	{
		DD_ASSERT( IsValid() );

		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_previousRead );

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo );
		CheckOGLError();
	}

	void FrameBuffer::BindDraw()
	{
		DD_ASSERT( IsValid() );

		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &m_previousDraw );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );

		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		m_previousSize = glm::vec2( viewport[2], viewport[3] );
		CheckOGLError();

		glViewport( 0, 0, m_texColour->GetSize().x, m_texColour->GetSize().y );
		CheckOGLError();
	}
	
	void FrameBuffer::UnbindRead()
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		CheckOGLError();
	}

	void FrameBuffer::UnbindDraw()
	{
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_previousDraw );
		glViewport( 0, 0, m_previousSize.x, m_previousSize.y );
		CheckOGLError();
	}
}