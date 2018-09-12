#pragma once

#include "Renderer.h"
#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct RayRenderer : Renderer
	{
		RayRenderer();
		~RayRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const ddr::RenderData& data ) override;

	private:

		ShaderHandle m_shader;
		VAO m_vao;
		VBO m_vbo;
	};
}