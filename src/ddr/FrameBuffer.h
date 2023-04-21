//
// FrameBuffer.h - A wrapper around OpenGL frame buffers used to render to a texture.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "ddr/OpenGL.h"
#include "ddr/RenderState.h"
#include "ddr/Shader.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	struct ICamera;
	struct Texture;
	struct UniformStorage;

	struct IFrameBuffer
	{
		virtual bool IsValid() const = 0;
		virtual GLuint ID() const = 0;
		virtual glm::ivec2 GetSize() const = 0;
	};

	struct FrameBuffer : IFrameBuffer
	{
		FrameBuffer();
		FrameBuffer(const FrameBuffer& other);
		~FrameBuffer();

		FrameBuffer& operator=(const FrameBuffer& other);

		//
		// Create a buffer that targets the given texture, with an optional depth texture.
		//
		bool Create(Texture& target, Texture* depth);

		//
		// Destroy the framebuffer.
		//
		void Destroy();
		void BindRead();
		void UnbindRead();

		void BindDraw();
		void UnbindDraw();

		void Clear();

		bool IsValid() const override { return m_valid; }

		GLuint ID() const override { return m_fbo; }

		glm::ivec2 GetSize() const override;

		Texture* GetColourTexture() const { return m_texColour; }
		Texture* GetDepthTexture() const { return m_texDepth; }

		void Initialize(std::string_view name);

		void Render(UniformStorage& uniforms);
		void RenderDepth(UniformStorage& uniforms, const ICamera& camera);

		void Blit(const IFrameBuffer& dest);
		void BlitToTexture(UniformStorage& uniforms, Texture* texture);

		void SetClearColour(glm::vec4 colour) { m_clearColour = colour; }
		void SetClearDepth(float depth) { m_clearDepth = depth; }

	private:

		std::string m_name;
		bool m_valid { false };
		GLuint m_fbo { OpenGL::InvalidID };

		glm::vec4 m_clearColour { 0, 0, 0, 0 };
		float m_clearDepth { 0.0f };

		glm::ivec2 m_previousSize { 0, 0 };
		GLint m_previousDraw { 0 };
		GLint m_previousRead { 0 };

		Texture* m_texColour { nullptr };
		Texture* m_texDepth { nullptr };
	};

	struct BackBuffer : IFrameBuffer
	{
		bool IsValid() const override { return true; }
		GLuint ID() const override { return 0u; }
		glm::ivec2 GetSize() const override;
	};
}