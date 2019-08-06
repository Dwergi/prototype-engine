#pragma once

#include "Mesh.h"

namespace neut
{
	struct WaterComponent
	{
		static const int VertexCount;

		ddr::MeshHandle Mesh;
		glm::vec2 TerrainChunkPosition { 0, 0 };
		std::vector<glm::vec3> Vertices;
		int LOD { 0 };
		bool Dirty { false };

		DD_BEGIN_CLASS( neut::WaterComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}