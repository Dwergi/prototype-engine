//
// WaterRenderer.h - Renderer for water.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "IRenderer.h"
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
	struct WaterRenderer : ddr::IRenderer
	{
		WaterRenderer();

		virtual void Initialize() override;
		virtual void Update(ddr::RenderData& render_data) override;
		virtual void Render(const ddr::RenderData& render_data) override;

	private:
		ddr::RenderState m_renderState;
	};
}