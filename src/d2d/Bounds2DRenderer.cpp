//
// Bounds2DRenderer.cpp
// Copyright (C) Sebastian Nordgren 
// August 13th 2018
//

#include "PCH.h"
#include "Bounds2DRenderer.h"

namespace d2d
{
	void Bounds2DRenderer::Render(const ddr::RenderData& render_data)
	{
	}

	void Bounds2DRenderer::DrawDebugInternal()
	{
		ImGui::Checkbox("Enable", &m_enabled);
	}
}