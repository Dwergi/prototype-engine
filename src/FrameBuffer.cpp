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

namespace ddr
{
	VAO FrameBuffer::m_vaoFullscreen;
	VBO FrameBuffer::m_vboFullscreen;
	ShaderHandle FrameBuffer::m_blitShader;

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
		CheckGLError();

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
		CheckGLError();

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColour->ID(), 0 );
		CheckGLError();

		GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers( m_fbo, 1, draw_buffers );
		CheckGLError();

		if( depth != nullptr )
		{
			m_texDepth = depth;

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texDepth->ID(), 0 );
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
		if( !m_blitShader.Valid() )
		{
			dd::Vector<Shader*> shaders;
			shaders.Add( Shader::Create( dd::String32( "shaders\\blit.vertex" ), Shader::Type::Vertex ) );
			shaders.Add( Shader::Create( dd::String32( "shaders\\blit.pixel" ), Shader::Type::Pixel ) );
			
			m_blitShader = ShaderProgram::Create( dd::String8( "blit" ), shaders );
			ShaderProgram* shader = ShaderProgram::Get( m_blitShader );

			shader->Use( true );

			m_vaoFullscreen.Create();
			m_vaoFullscreen.Bind();

			m_vboFullscreen.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
			m_vboFullscreen.Bind();
			m_vboFullscreen.SetData( s_fullScreenQuadBuffer );

			shader->BindPositions();

			m_vaoFullscreen.Unbind();

			shader->Use( false );
		}
	}

	void FrameBuffer::RenderDepth( const dd::ICamera& camera )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_texDepth != nullptr );

		ShaderProgram* shader = ShaderProgram::Get( m_blitShader );
		shader->Use( true );

		m_vaoFullscreen.Bind();
		m_texDepth->Bind( 0 );

		glDisable( GL_DEPTH_TEST );
		CheckGLError();

		shader->SetUniform( "Texture", *m_texDepth );
		shader->SetUniform( "Near", camera.GetNear() );
		shader->SetUniform( "DrawDepth", true );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckGLError();

		glEnable( GL_DEPTH_TEST );

		m_texDepth->Unbind();
		m_vaoFullscreen.Unbind();

		shader->Use( false );
		CheckGLError();
	}

	void FrameBuffer::Render()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_texColour != nullptr );

		ShaderProgram* shader = ShaderProgram::Get( m_blitShader );

		shader->Use( true );

		m_vaoFullscreen.Bind();
		
		m_texColour->Bind( 0 );

		glDisable( GL_DEPTH_TEST );
		
		shader->SetUniform( "Texture", *m_texColour );
		shader->SetUniform( "DrawDepth", false );

		glDrawArrays( GL_TRIANGLES, 0, s_fullScreenQuadBuffer.Size() );
		CheckGLError();

		glEnable( GL_DEPTH_TEST );

		m_texColour->Unbind();

		m_vaoFullscreen.Unbind();

		shader->Use( false );
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
			0, 0, m_texColour->GetSize().x, m_texColour->GetSize().y,
			0, 0, viewport[2], viewport[3],
			GL_COLOR_BUFFER_BIT, GL_LINEAR );
		CheckGLError();

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		CheckGLError();
	}

	void FrameBuffer::Destroy()
	{
		UnbindRead();
		UnbindDraw();

		if( m_fbo != OpenGL::InvalidID )
		{
			glDeleteBuffers( 1, &m_fbo );
			CheckGLError();

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
		CheckGLError();
	}

	void FrameBuffer::BindRead()
	{
		DD_ASSERT( IsValid() );

		glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_previousRead );

		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo );
		CheckGLError();
	}

	void FrameBuffer::BindDraw()
	{
		DD_ASSERT( IsValid() );

		glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &m_previousDraw );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );

		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		m_previousSize = glm::vec2( viewport[2], viewport[3] );
		CheckGLError();

		glViewport( 0, 0, m_texColour->GetSize().x, m_texColour->GetSize().y );
		CheckGLError();
	}
	
	void FrameBuffer::UnbindRead()
	{
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_previousRead );
		CheckGLError();
	}

	void FrameBuffer::UnbindDraw()
	{
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_previousDraw );
		glViewport( 0, 0, m_previousSize.x, m_previousSize.y );
		CheckGLError();
	}
}