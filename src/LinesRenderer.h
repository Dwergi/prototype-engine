//
// LinesRenderer.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct LinesRenderer : Renderer
	{
		LinesRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

	private:

		VAO m_vao;
		VBO m_vboPositions;
	};
}