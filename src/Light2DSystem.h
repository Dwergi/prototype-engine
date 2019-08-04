#pragma once

#include "Renderer.h"

namespace lux
{
	struct Light2DSystem : ddr::Renderer
	{
		Light2DSystem();

		void Update(const ddc::RenderData& render_data);
	}
}