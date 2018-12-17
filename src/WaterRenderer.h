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

namespace ddr
{
	struct WaterRenderer : Renderer
	{
		WaterRenderer( dd::JobSystem& jobsystem );

		virtual void RenderInit( ddc::World& world ) override;
		virtual void RenderUpdate( ddc::World& world ) override;
		virtual void Render( const RenderData& render_data ) override;

	private:

		dd::JobSystem& m_jobSystem;
		RenderState m_renderState;
	};
}