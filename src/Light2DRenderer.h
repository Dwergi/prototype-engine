#pragma once

#include "Renderer.h"
#include "Shader.h"

namespace d2d
{
	struct LightRenderer : ddr::Renderer
	{
		LightRenderer();

		virtual void Render(const ddr::RenderData& render_data) override;
		virtual void RenderInit(ddc::EntitySpace& entities) override;

	private: 
		ddr::ShaderHandle m_shader;
	};
}