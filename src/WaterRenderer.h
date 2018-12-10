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

namespace ddr
{
	struct WaterRenderer : Renderer
	{
		WaterRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual void RenderUpdate( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

	private:

		static ddr::ShaderHandle s_shader;
		static ddr::MaterialHandle s_material;

		RenderState m_renderState;
	};
}