#pragma once

#include "Mesh.h"

namespace dd
{
	struct WaterComponent
	{
		static const int VertexCount;

		glm::vec2 TerrainChunkPosition;
		std::vector<glm::vec3> Vertices;
		int LOD { 0 };
		bool Dirty { false };

		DD_CLASS( dd::WaterComponent )
		{
			DD_COMPONENT();
		}
	};
}