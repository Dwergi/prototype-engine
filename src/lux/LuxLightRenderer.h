#pragma once

#include "IRenderer.h"

#include "ddr/Shader.h"

namespace lux
{
	struct LuxLightRenderer : ddr::IRenderer
	{
		LuxLightRenderer();

		virtual void Initialize() override;
		virtual void Render(const ddr::RenderData& render_data) override;

	private: 
		ddr::ShaderHandle m_shader;
	};
}