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
	struct EntitySpace;
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
		virtual void RenderInit( ddc::EntitySpace& entities ) override;

		//
		// Update terrain chunks on the render thread.
		//
		virtual void RenderUpdate( ddc::EntitySpace& entities ) override;

		//
		// Commit uniforms to the renderer.
		//
		virtual void Render( const ddr::RenderData& data ) override;

		//
		// The name to display in the debug view list.
		//
		virtual const char* GetDebugTitle() const override { return "Wireframe"; }

	private:

		const dd::TerrainParameters& m_params;
		Wireframe* m_wireframe { nullptr };

		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;
	};
}