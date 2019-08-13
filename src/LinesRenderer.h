//
// LinesRenderer.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "IRenderer.h"
#include "RenderState.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct LinesRenderer : IRenderer
	{
		LinesRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void Render( const RenderData& render_data ) override;

	private:

		RenderState m_renderState;
		ShaderHandle m_shader;

		VAO m_vao;
		VBO m_vboPosition;
	};
}