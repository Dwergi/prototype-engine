//
// SpriteRenderer.h - A sprite renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#pragma once

#include "Renderer.h"
#include "RenderState.h"
#include "SpriteComponent.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	const int MAX_SPRITES = 1024;

	struct SpriteRenderer : ddr::Renderer
	{
	public:

		SpriteRenderer();

		virtual void RenderInit(ddc::World& world) override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render(const ddr::RenderData& data);

	private:
		RenderState m_renderState;

		VAO m_vao;

		std::vector<glm::vec2> m_positions;
		VBO m_vboPositions;

		std::vector<glm::vec2> m_sizes;
		VBO m_vboSizes;

		std::vector<glm::vec2> m_uvOffsets;
		VBO m_vboUVOffsets;

		std::vector<glm::vec2> m_uvScales;
		VBO m_vboUVScales;

		std::vector<glm::vec4> m_colours;
		VBO m_vboColours;

		std::vector<dd::SpriteComponent> m_temp;

		using SpriteIterator = std::vector<dd::SpriteComponent>::iterator;
		void DrawLayer(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms);
		void DrawInstancedSprites(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms);
	};
}