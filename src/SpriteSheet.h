#pragma once

#include "HandleManager.h"
#include "Sprite.h"

namespace ddr
{
	struct SpriteSheet : dd::HandleTarget
	{
		SpriteSheet();

		int Count() const { return (int) m_sprites.size(); }

		ddr::SpriteHandle Get(int idx) const { return m_sprites[idx]; }
		ddr::SpriteHandle Get(int x, int y) const;

		static const char* TypeName() { return "Sprite Sheet"; }

		DD_BASIC_TYPE(ddr::SpriteSheet)

	private:
		friend struct SpriteSheetManager;

		void SetDimensions(int width, int height);
		void Add(ddr::SpriteHandle sprite_h, int x, int y);

		glm::ivec2 m_dimensions { 0, 0 };
		std::vector<ddr::SpriteHandle> m_sprites;
	};

	using SpriteSheetHandle = dd::Handle<SpriteSheet>;

	struct SpriteSheetManager : dd::HandleManager<SpriteSheet>
	{
		SpriteSheetManager(ddr::SpriteManager& spriteManager) : m_spriteManager(spriteManager) {}

		ddr::SpriteSheetHandle Load(std::string base_name, ddr::TextureHandle texture_h, glm::ivec2 sprite_size);

	private:
		using super = dd::HandleManager<SpriteSheet>;

		ddr::SpriteManager& m_spriteManager;
	};
}