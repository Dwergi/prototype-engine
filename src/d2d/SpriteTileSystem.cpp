#include "PCH.h"
#include "d2d/SpriteTileSystem.h"

#include "Math_dd.h"
#include "IWindow.h"

#include "d2d/BoxPhysicsComponent.h"
#include "d2d/CirclePhysicsComponent.h"
#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

namespace d2d
{
	static dd::Service<dd::IWindow> s_window;

	SpriteTileSystem::SpriteTileSystem(glm::ivec2 map_dimensions, int tile_size) :
		ddc::System("Sprite Tiles")
	{
		m_mapDimensions = map_dimensions;
		m_tileSize = tile_size;

		RequireTag(ddc::Tag::Visible);
		RequireWrite<d2d::SpriteComponent>();
		RequireRead<d2d::Transform2DComponent>();
		OptionalWrite<d2d::CirclePhysicsComponent>();
		OptionalWrite<d2d::BoxPhysicsComponent>();
	}

	int SpriteTileSystem::GetScalingFactor(glm::ivec2 window_size) const
	{
		const int x_pixels = m_tileSize * m_mapDimensions.x;
		const int y_pixels = m_tileSize * m_mapDimensions.y;
		const int scaling_factor = ddm::min(window_size.x / x_pixels, window_size.y / y_pixels);
		return scaling_factor;
	}

	void SpriteTileSystem::UpdateConstants()
	{
		glm::ivec2 window_size = s_window->GetSize();

		const int scaling_factor = GetScalingFactor(window_size);
		m_scaledTileSize = (float) m_tileSize * scaling_factor;

		const glm::ivec2 active_area = glm::vec2(scaling_factor * m_tileSize * m_mapDimensions.x, scaling_factor * m_tileSize * m_mapDimensions.y);
		m_edgeOffset = (window_size - active_area) / 2;
	}

	glm::vec2 SpriteTileSystem::CoordToPixels(glm::vec2 coord) const
	{
		DD_ASSERT(!ddm::IsNaN(coord));

		return glm::vec2(
			m_edgeOffset.x + coord.x * m_scaledTileSize + m_scaledTileSize / 2,
			m_edgeOffset.y + coord.y * m_scaledTileSize + m_scaledTileSize / 2);
	}

	glm::vec2 SpriteTileSystem::PixelsToCoord(glm::vec2 pixels) const
	{
		DD_ASSERT(!ddm::IsNaN(pixels));

		glm::vec2 half_tile( m_scaledTileSize / 2.0f );
		 
		return (pixels - m_edgeOffset - half_tile) / glm::vec2(m_scaledTileSize); 
	}

	void SpriteTileSystem::Update(const ddc::UpdateData& update_data)
	{
		UpdateConstants();

		DD_ASSERT(m_scaledTileSize != 0);

		const ddc::UpdateBufferView& update_buffer = update_data.Data();

		auto entities = update_buffer.Entities();
		auto sprites = update_buffer.Write<d2d::SpriteComponent>();
		auto transforms = update_buffer.Read<d2d::Transform2DComponent>();
		auto boxes = update_buffer.Write<d2d::BoxPhysicsComponent>();
		auto circles = update_buffer.Write<d2d::CirclePhysicsComponent>();

		for (int i = 0; i < update_buffer.Size(); ++i)
		{
			const d2d::Transform2DComponent& transform_cmp = transforms[i];
			d2d::SpriteComponent& sprite_cmp = sprites[i];

			glm::vec2 sprite_pos_px = CoordToPixels(transform_cmp.Position);
			sprite_cmp.Position = sprite_pos_px;

			glm::vec2 sprite_size_px = transform_cmp.Scale * m_scaledTileSize;
			sprite_cmp.Size = sprite_size_px;

			sprite_cmp.Rotation = transform_cmp.Rotation;

			if (boxes.Has(i))
			{
				boxes[i].HitBox = sprite_cmp.HitBox.GetTransformed(transform_cmp.Transform());
			}

			if (circles.Has(i))
			{
				glm::vec2 half_extents = sprite_cmp.HitBox.HalfExtents();
				
				circles[i].HitCircle.Radius = ddm::max(half_extents.x, half_extents.y);
				circles[i].HitCircle.Centre = transform_cmp.Position;
			}
		}
	}
}