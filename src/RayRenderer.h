//
// RayRenderer.h - Renderer for RayComponent.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "IRenderer.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct RayRenderer : IRenderer
	{
		RayRenderer();
		~RayRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& data ) override;

	private:

		ShaderHandle m_shader;
		VAO m_vao;
		VBO m_vbo;
	};
}