#pragma once

#include "Mesh.h"

namespace dd
{
	struct WaterComponent
	{
		static const int VertexCount;

		ddr::MeshHandle Mesh;
		glm::vec2 TerrainChunkPosition;
		std::vector<glm::vec3> Vertices;
		int LOD { 0 };
		bool Dirty { false };

		DD_BEGIN_CLASS( dd::WaterComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}