#include "PCH.h"
#include "d2d/SpriteTileSystem.h"

#include "Math_dd.h"
#include "IWindow.h"

#include "d2d/SpriteComponent.h"
#include "d2d/SpriteTileComponent.h" 

namespace d2d
{
	static dd::Service<dd::IWindow> s_window;

	SpriteTileSystem::SpriteTileSystem(glm::ivec2 map_dimensions, int tile_size) :
		ddc::System("Sprite Tiles")
	{
		m_mapDimensions = map_dimensions;
		m_tileSize = tile_size;

		RequireTag(ddc::Tag::Visible);
		RequireRead<d2d::SpriteTileComponent>();
		RequireWrite<d2d::SpriteComponent>();
	}

	int SpriteTileSystem::GetScalingFactor() const
	{
		glm::ivec2 window_size = s_window->GetSize();
		
		const int x_pixels = m_tileSize * m_mapDimensions.x;
		const int y_pixels = m_tileSize * m_mapDimensions.y;
		const int scaling_factor = ddm::min(window_size.x / x_pixels, window_size.y / y_pixels);
		return scaling_factor;
	}

	glm::vec2 SpriteTileSystem::CoordToPixels(glm::vec2 coord) const
	{
		glm::ivec2 window_size = s_window->GetSize();

		const int scaling_factor = GetScalingFactor();
		const int scaled_tile_size = m_tileSize * GetScalingFactor();

		const glm::ivec2 active_area = glm::vec2(scaling_factor * m_tileSize * m_mapDimensions.x, scaling_factor * m_tileSize * m_mapDimensions.y);
		glm::ivec2 edge_offset = (window_size - active_area) / 2;

		return glm::vec2(
			edge_offset.x + coord.x * scaled_tile_size,
			edge_offset.y + coord.y * scaled_tile_size);
	}

	glm::vec2 SpriteTileSystem::PixelsToCoord(glm::vec2 pixels) const
	{
		glm::ivec2 window_size = s_window->GetSize();

		const int scaling_factor = GetScalingFactor();
		const int scaled_tile_size = m_tileSize * GetScalingFactor();

		const glm::ivec2 active_area = glm::vec2(scaling_factor * m_tileSize * m_mapDimensions.x, scaling_factor * m_tileSize * m_mapDimensions.y);
		glm::ivec2 edge_offset = (window_size - active_area) / 2;

		return glm::vec2((pixels.x - edge_offset.x) / scaled_tile_size, (pixels.y - edge_offset.y) / scaled_tile_size);
	}

	void SpriteTileSystem::Update(const ddc::UpdateData& update_data)
	{
		auto sprite_tiles = update_data.Data().Read<d2d::SpriteTileComponent>();
		auto sprites = update_data.Data().Write<d2d::SpriteComponent>();
		auto entities = update_data.Data().Entities();

		const int scaled_tile_size = m_tileSize * GetScalingFactor();

		for (int i = 0; i < update_data.Data().Size(); ++i)
		{
			const d2d::SpriteTileComponent& sprite_tile_cmp = sprite_tiles[i];
			d2d::SpriteComponent& sprite_cmp = sprites[i];

			glm::vec2 sprite_pos_px = CoordToPixels(sprite_tile_cmp.Coordinate);

			sprite_cmp.Position = sprite_pos_px;

			glm::vec2 sprite_size_px = sprite_tile_cmp.Scale * (float) scaled_tile_size;
			sprite_cmp.Size = sprite_size_px;
		}
	}
}