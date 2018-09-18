//
// LightRenderer.h - Renderer that ensures that lights get passed to other renderers.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "Renderer.h"
#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct LightRenderer : Renderer, dd::IDebugPanel
	{
		LightRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

		virtual const char* GetDebugTitle() const override { return "Lights"; }

	private:

		VAO m_vao;
		VBO m_vboPosition;
		VBO m_vboIndex;

		ShaderHandle m_shader;

		bool m_createLight { false };
		ddc::Entity m_deleteLight;
		std::vector<ddc::Entity> m_debugLights;

		bool m_drawBounds { false };

		virtual void DrawDebugInternal( const ddc::World& world ) override;

		void UpdateDebugPointLights( ddc::World& world );
	};
}
