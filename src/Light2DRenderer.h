#pragma once

#include "Renderer.h"
#include "Shader.h"

namespace lux
{
	struct Light2DRenderer : ddr::Renderer
	{
		Light2DRenderer();

		virtual void Render(const ddr::RenderData& render_data) override;
		virtual void RenderInit(ddc::EntitySpace& entities) override;

	private: 
		ddr::ShaderHandle m_shader;
	};
}