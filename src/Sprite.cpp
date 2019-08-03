#include "PCH.h"
#include "Sprite.h"

#include <fmt/format.h>

DD_TYPE_CPP(ddr::Sprite);
DD_POD_CPP(ddr::SpriteHandle);

namespace ddr
{
	std::vector<SpriteHandle> SpriteManager::LoadSpriteSheet(std::string base_name, ddr::TextureHandle sprite_sheet_h, glm::ivec2 sprite_size)
	{
		DD_ASSERT(sprite_sheet_h.IsValid());

		ddr::Texture* sprite_sheet = sprite_sheet_h.Access();
		glm::ivec2 sprite_sheet_size = sprite_sheet->GetSize();

		DD_ASSERT(sprite_sheet_size.x % sprite_size.x == 0 && sprite_sheet_size.y % sprite_size.y == 0, "Sprite sheet misformed!");

		glm::vec2 size_norm = glm::vec2(sprite_size) / glm::vec2(sprite_sheet_size);

		std::vector<SpriteHandle> created_sprites;
		created_sprites.reserve(size_t(sprite_sheet_size.x / sprite_size.x) * size_t(sprite_sheet_size.y / sprite_size.y));

		for (int y_offset = 0; y_offset < sprite_sheet_size.y; y_offset += sprite_size.y)
		{
			for (int x_offset = 0; x_offset < sprite_sheet_size.x; x_offset += sprite_size.x)
			{
				int x_index = x_offset / sprite_size.x;
				int y_index = y_offset / sprite_size.y;
				std::string sprite_name = fmt::format("{}_{}x{}", base_name, x_index, y_index);
				
				SpriteHandle new_sprite_h = super::Create(sprite_name);
				Sprite* new_sprite = new_sprite_h.Access();

				new_sprite->Size = sprite_size;
				new_sprite->SizeNormalized = size_norm;

				new_sprite->Offset = glm::ivec2(x_offset, y_offset);

				glm::vec2 offset_norm = glm::vec2(new_sprite->Offset) / glm::vec2(sprite_sheet_size);
				new_sprite->OffsetNormalized = offset_norm;

				new_sprite->Texture = sprite_sheet_h;

				created_sprites.push_back(new_sprite_h);
			}
		}

		return created_sprites;
	}
}