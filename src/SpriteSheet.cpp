#include "PCH.h"
#include "SpriteSheet.h"

#include <fmt/format.h>

DD_TYPE_CPP(ddr::SpriteSheet);
DD_POD_CPP(ddr::SpriteSheetHandle);

namespace ddr
{

	SpriteSheet::SpriteSheet()
	{

	}

	ddr::SpriteHandle SpriteSheet::Get(int x, int y) const
	{
		DD_ASSERT(x >= 0 && x < m_dimensions.x);
		DD_ASSERT(y >= 0 && y < m_dimensions.y);

		return Get(y * m_dimensions.x + x);
	}

	void SpriteSheet::SetDimensions(int width, int height)
	{
		DD_ASSERT(m_sprites.empty());
		m_dimensions = glm::ivec2(width, height);

		m_sprites.reserve(width * height);
	}

	void SpriteSheet::Add(ddr::SpriteHandle sprite_h, int x, int y)
	{
		DD_ASSERT(x >= 0 && x < m_dimensions.x);
		DD_ASSERT(y >= 0 && y < m_dimensions.y);

		int index = y * m_dimensions.x + x;

		m_sprites.push_back(sprite_h);
	}

	SpriteSheetHandle SpriteSheetManager::Load(std::string base_name, ddr::TextureHandle texture_h, glm::ivec2 sprite_size)
	{
		DD_ASSERT(texture_h.IsValid());

		ddr::Texture* texture = texture_h.Access();
		glm::ivec2 texture_size = texture->GetSize();

		DD_ASSERT(texture_size.x % sprite_size.x == 0 && texture_size.y % sprite_size.y == 0, "Sprite sheet misformed!");

		const int width = texture_size.x / sprite_size.x;
		const int height = texture_size.y / sprite_size.y; 

		ddr::SpriteSheetHandle sprite_sheet_h = super::Create(base_name);
		ddr::SpriteSheet* sprite_sheet = sprite_sheet_h.Access();
		sprite_sheet->SetDimensions(width, height);

		glm::vec2 size_norm = glm::vec2(sprite_size) / glm::vec2(texture_size);

		for (int y_index = 0; y_index < height; ++y_index)
		{
			for (int x_index = 0; x_index < width; ++x_index)
			{
				int x_offset = x_index * sprite_size.x;
				int y_offset = y_index * sprite_size.y;
				std::string sprite_name = fmt::format("{}_{}x{}", base_name, x_index, y_index);

				SpriteHandle new_sprite_h = m_spriteManager.Create(sprite_name);
				Sprite* new_sprite = new_sprite_h.Access();

				new_sprite->Size = sprite_size;
				new_sprite->SizeNormalized = size_norm;

				new_sprite->Offset = glm::ivec2(x_offset, y_offset);

				glm::vec2 offset_norm = glm::vec2(new_sprite->Offset) / glm::vec2(texture_size);
				new_sprite->OffsetNormalized = offset_norm;

				new_sprite->Texture = texture_h;

				sprite_sheet->Add(new_sprite_h, x_index, y_index);
			}
		}

		return sprite_sheet_h;
	}
}