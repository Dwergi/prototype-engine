#pragma once

#include "ddm/AABB2D.h"
#include "Sprite.h"

namespace d2d
{
	struct SpriteComponent
	{
		SpriteComponent();
		SpriteComponent(const SpriteComponent& other);
		SpriteComponent& operator=(const SpriteComponent& other);

		// handle to a sprite
		ddr::SpriteHandle Sprite;
		
		// Z-index of the sprite - higher is more forward
		int ZIndex { 0 };

		// screen position of where to draw the sprite in pixels
		glm::vec2 Position { 0, 0 };

		// size in pixels
		glm::vec2 Size { 0, 0 };

		// colour to modulate sprite with
		glm::vec4 Colour { 1, 1, 1, 1 };

		// hit box in relation to the sprite size (default (0,0)-(1,1))
		ddm::AABB2D HitBox;

		DD_BEGIN_CLASS(d2d::SpriteComponent)
			DD_COMPONENT();

			DD_MEMBER(Sprite);
			DD_MEMBER(Position);
			DD_MEMBER(Size);
			DD_MEMBER(Colour);
			DD_MEMBER(ZIndex);
		DD_END_CLASS()
	};
}