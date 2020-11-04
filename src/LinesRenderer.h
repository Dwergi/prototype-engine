//
// LinesRenderer.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "IRenderer.h"

#include "ddr/RenderState.h"
#include "ddr/Shader.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	struct LinesRenderer : IRenderer
	{
		LinesRenderer();

		virtual void Initialize() override;
		virtual void Render( const ddr::RenderData& render_data ) override;

	private:

		RenderState m_renderState;
		ShaderHandle m_shader;

		VAO m_vao;
		VBO m_vboPosition;
	};
}