#pragma once

#include "Sprite.h"

namespace dd
{
	struct SpriteAnimationComponent
	{
		dd::Vector<ddr::SpriteHandle> Frames;
		int CurrentFrame { -1 };

		DD_BEGIN_CLASS(SpriteAnimationComponent)
			DD_MEMBER(Frames);
			DD_MEMBER(CurrentFrame);
		DD_END_CLASS()
	};
}