#pragma once

#include "Sprite.h"

namespace dd
{
	struct SpriteComponent
	{
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent& other);
		SpriteComponent& operator=(const SpriteComponent& other);

		// handle to a sprite
		ddr::SpriteHandle Sprite;
		
		// Z-index of the sprite - higher is more forward
		int ZIndex { 0 };

		// normalized device coordinates [-1, 1] of where to draw the sprite
		glm::vec2 Position { 0, 0 };

		// size in normalized device coordinates [-1, 1]
		glm::vec2 Size { 0, 0 };

		// colour to modulate sprite with
		glm::vec4 Colour { 1, 1, 1, 1 };

		DD_BEGIN_CLASS(dd::SpriteComponent)
			DD_COMPONENT();

			DD_MEMBER(Sprite);
			DD_MEMBER(Position);
			DD_MEMBER(Size);
			DD_MEMBER(Colour);
			DD_MEMBER(ZIndex);
		DD_END_CLASS()
	};
}