#pragma once

#include "Texture.h"

namespace ddr
{
	struct Sprite : dd::HandleTarget
	{
		// the texture of the sprite
		ddr::TextureHandle Texture;

		// offset into the texture image in pixels - maybe ditch?
		glm::ivec2 Offset { 0, 0 };

		// offset into the texture in range [0-1]
		glm::vec2 OffsetNormalized { 0, 0 };

		// size in pixels of the sprite in the image - maybe ditch?
		glm::ivec2 Size { 0, 0 };

		// size as percentage of texture size
		glm::vec2 SizeNormalized { 0, 0 };

		static const char* TypeName() { return "Sprite"; }

		DD_BEGIN_CLASS(ddr::Sprite)
			DD_MEMBER(Offset);
			DD_MEMBER(OffsetNormalized);
			DD_MEMBER(Size);
			DD_MEMBER(SizeNormalized);
			DD_MEMBER(Texture);
		DD_END_CLASS();
	};

	using SpriteHandle = dd::Handle<Sprite>;

	struct SpriteManager : dd::HandleManager<Sprite>
	{
	private:
		using super = dd::HandleManager<Sprite>;
	};
}