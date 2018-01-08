//
// RenderToTexture.h - A wrapper around OpenGL frame buffers used to render to a texture.
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
	class Texture;
	class Window;

	class RenderToTexture
	{
	public:

		RenderToTexture( const Window& window );
		RenderToTexture( const RenderToTexture& other );
		~RenderToTexture();

		RenderToTexture& operator=( const RenderToTexture& other );

		//
		// Create a buffer with dimensions and whether or not to generate a depth buffer.
		//
		bool Create( Texture& target, bool depth );

		//
		// Destroy the framebuffer.
		//
		void Destroy();
		
		void Bind();
		void Unbind();

		bool IsValid() const { return m_valid; }

		Texture* GetTexture() const { return m_texture; }

		void PreRender();

		void Render();

		void SetClearColour( glm::vec4 clear_colour ) { m_clearColour = clear_colour; }

	private:

		const Window* m_window;
		bool m_valid { false };
		GLuint m_frameBuffer { OpenGL::InvalidID };
		GLuint m_depth { OpenGL::InvalidID };
		glm::vec4 m_clearColour;

		Texture* m_texture { nullptr };

		static ShaderHandle m_blitShader;
		static VBO m_fullScreenVBO;
		static VAO m_fullScreenVAO;
	};
}