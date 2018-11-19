#pragma once

#include "AABB.h"
#include "TerrainParameters.h"

namespace dd
{
	struct TerrainTile
	{
		static constexpr float TileSize = 32.0f;
		static constexpr uint MaxLODs = 8;
		static constexpr uint MaxVertices = 256;
		static constexpr float VertexDistance = 0.25f;

		void Initialize();

	private:

		const TerrainParameters m_terrainParams;
		static std::vector<dd::Buffer<uint>> s_indices;

		static constexpr uint MeshVertexCount = (MaxVertices + 1) * (MaxVertices + 1);
		static constexpr uint FlapVertexCount = (MaxVertices + 1) * 4;
		static constexpr uint TotalVertexCount = MeshVertexCount + FlapVertexCount;

		glm::vec3 m_positions[ TotalVertexCount ];
		dd::Buffer<glm::vec3> m_positionsBuffer;

		dd::AABB m_bounds;
		int m_highestLOD { 1000 };

		static void GenerateIndices( uint lod );

		void GeneratePositions( glm::vec2 tile_pos, uint lod );
		float GetHeight( float x, float y ) const;
	};
}