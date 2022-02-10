//
// SpriteRenderer.h - A sprite renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"

#include "ddr/RenderState.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

#include "d2d/SpriteComponent.h"

namespace ddr
{
	const int MAX_SPRITES = 1024;

	struct SpriteRenderer : ddr::IRenderer, dd::IDebugPanel
	{
	public:

		SpriteRenderer();

		virtual void Initialize() override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render(const ddr::RenderData& data);

	private:
		RenderState m_renderState;

		struct InstanceVBOs
		{
			VAO VAO;
			VBO Transforms;
			VBO Colours;
			VBO UVOffsets;
			VBO UVScales;
		};

		std::unordered_map<ddr::SpriteHandle, InstanceVBOs> m_instanceCache;

		InstanceVBOs& FindCachedInstanceVBOs(ddr::SpriteHandle sprite_h);

		std::vector<glm::mat3> m_transforms;
		std::vector<glm::vec2> m_uvOffsets;
		std::vector<glm::vec2> m_uvScales;
		std::vector<glm::vec4> m_colours;

		std::vector<d2d::SpriteComponent> m_temp;

		using SpriteIterator = std::vector<d2d::SpriteComponent>::iterator;
		void DrawLayer(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms, ddr::Shader& shader);
		void DrawInstancedSprites(SpriteIterator start, SpriteIterator end, ddr::UniformStorage& uniforms, ddr::Shader& shader);

		virtual const char* GetDebugTitle() const override { return "Sprites"; }
		virtual void DrawDebugInternal() override;
	};
}