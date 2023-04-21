//
// LightRenderer.h - Renderer that ensures that lights get passed to other renderers.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"

#include "ddr/Mesh.h"
#include "ddr/Shader.h"

namespace ddr
{
	struct LightRenderer : IRenderer, dd::IDebugPanel
	{
		LightRenderer();

		virtual void Initialize() override;
		virtual void Update(ddr::RenderData& data) override;
		virtual void Render(const ddr::RenderData& render_data) override;

		virtual const char* GetDebugTitle() const override { return "Lights"; }

	private:

		MeshHandle m_mesh;
		ddr::ShaderHandle m_shader;

		bool m_createLight { false };
		ddc::Entity m_deleteLight;
		std::vector<ddc::Entity> m_debugLights;

		virtual void DrawDebugInternal() override;

		void UpdateDebugPointLights( ddc::EntityLayer& entities );
	};
}
