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
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f,  1.0f, 0.0f),
		glm::vec3(-1.0f,  1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f)
	};

	static const dd::ConstBuffer<glm::vec3> s_fullScreenQuadBuffer(s_fullScreenQuadVertices, 6);

	FrameBuffer::FrameBuffer()
	{

	}

	FrameBuffer::FrameBuffer(const FrameBuffer& other)
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

	FrameBuffer& FrameBuffer::operator=(const FrameBuffer& other)
	{
		m_valid = other.m_valid;
		m_fbo = other.m_fbo;
		m_texColour = other.m_texColour;
		m_texDepth = other.m_texDepth;

		return *this;
	}

	glm::ivec2 FrameBuffer::GetSize() const
	{
		if (m_texColour != nullptr)
		{
			return m_texColour->GetSize();
		}
		else if (m_texDepth != nullptr)
		{
			return m_texDepth->GetSize();
		}
		return glm::ivec2(0, 0);
	}

	bool FrameBuffer::Create(Texture& target, Texture* depth)
	{
		m_texColour = &target;

		glCreateFramebuffers(1, &m_fbo);
		CheckOGLError();

		glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, m_texColour->ID(), 0);
		CheckOGLError();

		GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers(m_fbo, 1, draw_buffers);
		CheckOGLError();

		if (depth != nullptr)
		{
			m_texDepth = depth;

			glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_texDepth->ID(), 0);
			CheckOGLError();
		}

		GLuint status = glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			DD_ASSERT(status != GL_FRAMEBUFFER_COMPLETE, "Failed to create framebuffer!");
			return false;
		}

		m_valid = true;
		return true;
	}

	void FrameBuffer::Initialize()
	{
		if (!m_blitShader.IsValid())
		{
			m_blitShader = s_shaderManager->Load("blit");

			ScopedShader shader = m_blitShader.Access()->UseScoped();

			m_vaoFullscreen.Create();

			m_vboFullscreen.Create(s_fullScreenQuadBuffer);
			m_vaoFullscreen.BindVBO(m_vboFullscreen, 0, sizeof(s_fullScreenQuadBuffer[0]));

			shader->BindPositions(m_vaoFullscreen, m_vboFullscreen);
		}
	}

	void FrameBuffer::RenderDepth(ddr::UniformStorage& uniforms, const ddr::ICamera& camera)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_texDepth != nullptr);

		ScopedShader shader = m_blitShader.Access()->UseScoped();

		m_vaoFullscreen.Bind();
		m_texDepth->Bind(0);

		CheckOGLError();

		uniforms.Set("Texture", *m_texDepth);
		uniforms.Set("Near", camera.GetNear());
		uniforms.Set("DrawDepth", true);

		uniforms.Upload(*shader);

		OpenGL::DrawArrays(OpenGL::Primitive::Triangles, s_fullScreenQuadBuffer.Size());

		m_texDepth->Unbind();
		m_vaoFullscreen.Unbind();

		CheckOGLError();
	}

	void FrameBuffer::Render(ddr::UniformStorage& uniforms)
	{
		BlitToTexture(uniforms, m_texColour);
	}

	void FrameBuffer::BlitToTexture(ddr::UniformStorage& uniforms, ddr::Texture* texture)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_texColour != nullptr);

		ScopedShader shader = m_blitShader.Access()->UseScoped();

		m_vaoFullscreen.Bind();

		texture->Bind(0);

		uniforms.Set("Texture", *texture);
		uniforms.Set("DrawDepth", false);

		uniforms.Upload(*shader);

		OpenGL::DrawArrays(OpenGL::Primitive::Triangles, s_fullScreenQuadBuffer.Size());

		texture->Unbind();

		m_vaoFullscreen.Unbind();
	}

	void FrameBuffer::Blit(const IFrameBuffer& dest)
	{
		glm::ivec2 dest_size = dest.GetSize();

		glBlitNamedFramebuffer(ID(), dest.ID(),
			0, 0, GetSize().x, GetSize().y,
			0, 0, dest_size.x, dest_size.y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
		CheckOGLError();
	}

	void FrameBuffer::Destroy()
	{
		if (m_fbo != OpenGL::InvalidID)
		{
			glDeleteBuffers(1, &m_fbo);
			CheckOGLError();

			m_fbo = OpenGL::InvalidID;
		}

		m_texDepth = nullptr;
		m_texColour = nullptr;
		m_valid = false;
	}

	void FrameBuffer::Clear()
	{
		glClearNamedFramebufferfv(m_fbo, GL_COLOR, 0, (const GLfloat*) &m_clearColour.data);
		glClearNamedFramebufferfv(m_fbo, GL_DEPTH, 0, &m_clearDepth);
		CheckOGLError();
	}

	void FrameBuffer::BindRead()
	{
		DD_ASSERT(IsValid());

		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_previousRead);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
		CheckOGLError();
	}

	void FrameBuffer::BindDraw()
	{
		DD_ASSERT(IsValid());

		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_previousDraw);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		m_previousSize = glm::vec2(viewport[2], viewport[3]);
		CheckOGLError();

		glViewport(0, 0, m_texColour->GetSize().x, m_texColour->GetSize().y);
		CheckOGLError();
	}

	void FrameBuffer::UnbindRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_previousRead);
		m_previousRead = 0;

		CheckOGLError();
	}

	void FrameBuffer::UnbindDraw()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_previousDraw);
		m_previousDraw = 0;

		glViewport(0, 0, m_previousSize.x, m_previousSize.y);
		m_previousSize = { 0, 0 };

		CheckOGLError();
	}

	glm::ivec2 BackBuffer::GetSize() const
	{
		static dd::Service<dd::IWindow> s_window;

		return s_window->GetSize();
	}
}
