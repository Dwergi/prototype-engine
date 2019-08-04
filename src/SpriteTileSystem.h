#pragma once

#include "System.h"

namespace lux
{
	struct SpriteTileSystem : ddc::System
	{
		SpriteTileSystem(glm::ivec2 map_dimensions, int tile_size);

		virtual void Update(const ddc::UpdateData& update_data) override;

		glm::vec2 CoordToPixels(glm::vec2 coord) const;
		glm::vec2 PixelsToCoord(glm::vec2 coord) const;

	private:

		glm::ivec2 m_mapDimensions { 0, 0 };
		int m_tileSize { 0 };

		int GetScalingFactor() const;
	};
}