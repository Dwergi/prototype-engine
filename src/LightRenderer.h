//
// LightRenderer.h - Renderer that ensures that lights get passed to other renderers.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "MeshHandle.h"
#include "Renderer.h"
#include "ShaderHandle.h"

namespace ddr
{
	struct LightRenderer : Renderer, dd::IDebugPanel
	{
		LightRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

		virtual const char* GetDebugTitle() const override { return "Lights"; }

	private:

		MeshHandle m_mesh;

		ShaderHandle m_shader;

		bool m_createLight { false };
		ddc::Entity m_deleteLight;
		std::vector<ddc::Entity> m_debugLights;

		bool m_drawBounds { false };

		virtual void DrawDebugInternal( const ddc::World& world ) override;

		void UpdateDebugPointLights( ddc::World& world );
	};
}
