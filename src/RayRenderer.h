//
// RayRenderer.h - Renderer for RayComponent.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "IRenderer.h"

#include "ddr/Shader.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	struct RayRenderer : IRenderer
	{
		RayRenderer();
		~RayRenderer();

		virtual void Initialize() override;
		virtual void Render( const ddr::RenderData& data ) override;

	private:

		ddr::ShaderHandle m_shader;
		VAO m_vao;
		VBO m_vbo;
	};
}