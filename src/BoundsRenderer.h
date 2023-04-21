//
// BoundsRenderer.h - A debug renderer for bounds.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"

#include "ddr/RenderState.h"
#include "ddr/Shader.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace dd
{
	struct InputKeyBindings;
}

namespace ddr
{
	struct BoundsRenderer : ddr::IRenderer, dd::IDebugPanel
	{
		BoundsRenderer();
		~BoundsRenderer();

		virtual void Initialize() override;
		virtual void Render(const ddr::RenderData& data) override;
		virtual bool UsesAlpha() const { return true; }

	private:

		enum class DrawMode
		{
			None,
			Box,
			Sphere
		};

		DrawMode m_drawMode { DrawMode::None };
		bool m_updateBuffers { false };
		int m_subdivisions { 1 };

		VAO m_vao;
		VBO m_vboPosition;
		VBO m_vboIndex;

		RenderState m_renderState;
		ddr::ShaderHandle m_shader;

		virtual const char* GetDebugTitle() const override { return "Bounds"; }
		virtual void DrawDebugInternal() override;

		void UpdateBuffers();
	};
}
