#pragma once

#include "IRenderer.h"
#include "Shader.h"

namespace lux
{
	struct LuxLightRenderer : ddr::IRenderer
	{
		LuxLightRenderer();

		virtual void Render(const ddr::RenderData& render_data) override;
		virtual void RenderInit(ddc::EntitySpace& entities) override;

	private: 
		ddr::ShaderHandle m_shader;
	};
}