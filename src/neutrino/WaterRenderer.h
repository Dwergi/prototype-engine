//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "Renderer.h"
#include "RenderState.h"
#include "Material.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

namespace dd
{
	struct JobSystem;
}

namespace neut
{
	struct WaterRenderer : ddr::Renderer
	{
		WaterRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void RenderUpdate( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& render_data ) override;

	private:
		ddr::RenderState m_renderState;
	};
}