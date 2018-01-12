//
// FrameBuffer.h - A wrapper around OpenGL frame buffers used to render to a texture.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "OpenGL.h"
#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"

namespace dd
{
	class ICamera;
	class Texture;

	class FrameBuffer
	{
	public:

		FrameBuffer();
		FrameBuffer( const FrameBuffer& other );
		~FrameBuffer();

		FrameBuffer& operator=( const FrameBuffer& other );

		//
		// Create a buffer that targets the given texture, with an optional depth texture.
		//
		bool Create( Texture& target, Texture* depth );

		//
		// Destroy the framebuffer.
		//
		void Destroy();
		
		void Bind();
		void Unbind();

		bool IsValid() const { return m_valid; }

		GLuint ID() const { return m_fbo; }

		Texture* GetColourTexture() const { return m_colour; }
		Texture* GetDepthTexture() const { return m_depth; }

		void RenderInit();

		void Render();
		void RenderDepth();

		void Blit();

		void SetClearColour( glm::vec4 colour ) { m_clearColour = colour; }
		void SetClearDepth( float depth ) { m_clearDepth = depth; }

	private:

		bool m_valid { false };
		GLuint m_fbo { OpenGL::InvalidID };

		glm::vec4 m_clearColour;
		float m_clearDepth { 0.0f };

		glm::ivec2 m_previousSize;
		GLint m_previousDraw { -1 };
		GLint m_previousRead { -1 };

		Texture* m_colour { nullptr };
		Texture* m_depth { nullptr };

		static ShaderHandle m_blitShader;
		static VBO m_fullScreenVBO;
		static VAO m_fullScreenVAO;
	};
}