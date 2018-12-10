//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "Renderer.h"
#include "RenderState.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct WaterRenderer : Renderer
	{
		WaterRenderer();

		virtual void RenderUpdate( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

	private:

		RenderState m_renderState;
	};
}