#pragma once

#include "Mesh.h"

namespace dd
{
	struct WaterComponent
	{
		glm::vec2 TerrainChunkPosition;

		DD_CLASS( dd::WaterComponent )
		{
			DD_COMPONENT();
		}
	};
}