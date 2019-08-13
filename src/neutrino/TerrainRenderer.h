//
// TerrainRenderer.h
// Copyright (C) Sebastian Nordgren 
// September 30th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"

namespace ddc
{
	struct EntitySpace;
}

namespace neut
{
	struct TerrainParameters;
	struct Wireframe;

	struct TerrainRenderer : ddr::IRenderer, dd::IDebugPanel
	{
		TerrainRenderer( const neut::TerrainParameters& params );

		//
		// Initialize render resources for the terrain system.
		//
		virtual void Initialize() override;

		//
		// Update terrain chunks on the render thread.
		//
		virtual void Update(ddr::RenderData& data) override;

		//
		// Commit uniforms to the renderer.
		//
		virtual void Render( const ddr::RenderData& data ) override;

		//
		// The name to display in the debug view list.
		//
		virtual const char* GetDebugTitle() const override { return "Wireframe"; }

	private:

		const neut::TerrainParameters& m_params;
		neut::Wireframe* m_wireframe { nullptr };

		virtual void DrawDebugInternal() override;
	};
}