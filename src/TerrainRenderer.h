//
// TerrainRenderer.h
// Copyright (C) Sebastian Nordgren 
// September 30th 2018
//

#pragma once

#include "Renderer.h"

namespace dd
{
	struct TerrainParameters;
}

namespace ddc
{
	struct World;
}

namespace ddr
{
	struct Wireframe;

	struct TerrainRenderer : Renderer, dd::IDebugPanel
	{
		TerrainRenderer( const dd::TerrainParameters& params );

		//
		// Initialize render resources for the terrain system.
		//
		virtual void RenderInit( ddc::World& world ) override;

		//
		// Update terrain chunks on the render thread.
		//
		virtual void Render( const ddr::RenderData& data ) override;

		//
		// The name to display in the debug view list.
		//
		virtual const char* GetDebugTitle() const override { return "Terrain"; }

	private:

		const dd::TerrainParameters& m_params;
		Wireframe* m_wireframe { nullptr };

		virtual void DrawDebugInternal( ddc::World& world ) override;
	};
}