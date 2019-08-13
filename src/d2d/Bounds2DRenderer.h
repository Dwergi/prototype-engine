//
// Bounds2DRenderer.h
// Copyright (C) Sebastian Nordgren 
// August 13th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"
 
namespace d2d
{
	struct Bounds2DRenderer : ddr::IRenderer, dd::IDebugPanel
	{
	private:

		bool m_enabled { false };

		// IRenderer implementation
		virtual void Initialize();
		virtual void Render(const ddr::RenderData& render_data) override;

		// IDebugPanel implementation
		virtual const char* GetDebugTitle() const override { return "2D Bounds"; }
		virtual void DrawDebugInternal() override;
	};
}