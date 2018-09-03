#pragma once

#include "Renderer.h"

namespace ddr
{
	struct LightRenderer : Renderer
	{
		LightRenderer();

		virtual void Render( const RenderData& render_data ) override;
	};
}
