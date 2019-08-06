#pragma once

namespace d2d
{
	struct SpriteTileComponent
	{
		// tile coordinate, top-left is (0,0)
		glm::vec2 Coordinate { 0, 0 };

		// scale of the tile, so eg. (2,2) occupies 4 tiles
		glm::vec2 Scale { 1, 1 };

		DD_BEGIN_CLASS(d2d::SpriteTileComponent)
			DD_COMPONENT();

			DD_MEMBER(Coordinate); 
			DD_MEMBER(Scale);
		DD_END_CLASS()
	};
}