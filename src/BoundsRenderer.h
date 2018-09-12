#pragma once

#include "IDebugPanel.h"
#include "Renderer.h"
#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct BoundsRenderer : ddr::Renderer, dd::IDebugPanel
	{
		BoundsRenderer();
		~BoundsRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const ddr::RenderData& data ) override;
		virtual bool UsesAlpha() const { return true; }

	private:

		bool m_draw { false };

		VAO m_vao;
		VBO m_vboPosition;
		VBO m_vboIndex;

		ShaderHandle m_shader;

		virtual const char* GetDebugTitle() const override { return "Bounds"; }
		virtual void DrawDebugInternal( const ddc::World& world ) override;
	};
}