#pragma once

#include "ddm/AABB2D.h"
#include "Sprite.h"

namespace d2d
{
	struct SpriteComponent
	{
		SpriteComponent();

		// handle to a sprite
		ddr::SpriteHandle Sprite;
		
		// Z-index of the sprite - higher is more forward
		int ZIndex { 0 };

		// screen position of where to draw the sprite in pixels
		glm::vec2 Position { 0, 0 };

		// size in pixels
		glm::vec2 Size { 0, 0 };

		// the point in the sprite to pivot around
		glm::vec2 Pivot { 0, 0 };

		// rotation in radians
		float Rotation { 0 };

		// colour to modulate sprite with
		glm::vec4 Colour { 1, 1, 1, 1 };

		// hit box in relation to the sprite size (default (0,0)-(1,1))
		ddm::AABB2D HitBox;

		DD_BEGIN_CLASS(d2d::SpriteComponent)
			DD_COMPONENT();

			DD_MEMBER(Sprite);
			DD_MEMBER(Position);
			DD_MEMBER(Size);
			DD_MEMBER(Rotation);
			DD_MEMBER(Colour);
			DD_MEMBER(ZIndex);
			DD_MEMBER(HitBox);
		DD_END_CLASS()
	};
}