#pragma once

#include "System.h"

namespace d2d
{
	struct SpriteTileSystem : ddc::System
	{
		SpriteTileSystem(glm::ivec2 map_dimensions, int tile_size);

		virtual void Update(ddc::UpdateData& update_data) override;

		glm::vec2 CoordToPixels(glm::vec2 coord) const;
		glm::vec2 PixelsToCoord(glm::vec2 coord) const;

	private:

		glm::ivec2 m_mapDimensions { 0, 0 };
		int m_tileSize { 0 };

		float m_scaledTileSize { 0 };
		glm::vec2 m_edgeOffset { 0, 0 };

		void UpdateConstants();
		int GetScalingFactor(glm::ivec2 window_size) const;
	};
}