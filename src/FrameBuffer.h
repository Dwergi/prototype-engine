//
// FrameBuffer.h - A wrapper around OpenGL frame buffers used to render to a texture.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "OpenGL.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct ICamera;
	struct Texture;
	struct UniformStorage;

	struct FrameBuffer
	{
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
		
		void BindRead();
		void UnbindRead();

		void BindDraw();
		void UnbindDraw();

		void Clear();

		bool IsValid() const { return m_valid; }

		GLuint ID() const { return m_fbo; }

		Texture* GetColourTexture() const { return m_texColour; }
		Texture* GetDepthTexture() const { return m_texDepth; }

		void RenderInit();

		void Render( ddr::UniformStorage& uniforms );
		void RenderDepth( ddr::UniformStorage& uniforms, const ddr::ICamera& camera );

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

		Texture* m_texColour { nullptr };
		Texture* m_texDepth { nullptr };

		static ShaderHandle m_blitShader;
		static VBO m_vboFullscreen;
		static VAO m_vaoFullscreen;
	};
}