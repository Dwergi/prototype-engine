#pragma once

#include "ddm/AABB2D.h"

namespace d2d
{
	struct SpriteTileComponent
	{
		// the position of the top-left of the tile in coordinate units
		glm::vec2 Coordinate;

		// scale of the tile, so eg. (2,2) occupies 4 tiles
		glm::vec2 Scale { 1, 1 };

		// hitbox relative to the sprite size [0-1]
		ddm::AABB2D HitBox;

		DD_BEGIN_CLASS(d2d::SpriteTileComponent)
			DD_COMPONENT();

			DD_MEMBER(Scale); 
			DD_MEMBER(HitBox);
		DD_END_CLASS()
	};
}