//
// BoundsRenderer.h - A debug renderer for bounds.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "Renderer.h"
#include "RenderState.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace dd
{
	struct InputKeyBindings;
}

namespace ddr
{
	struct BoundsRenderer : ddr::Renderer, dd::IDebugPanel
	{
		BoundsRenderer();
		~BoundsRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& data ) override;
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
		ShaderHandle m_shader;

		virtual const char* GetDebugTitle() const override { return "Bounds"; }
		virtual void DrawDebugInternal() override;

		void UpdateBuffers();
	};
}
