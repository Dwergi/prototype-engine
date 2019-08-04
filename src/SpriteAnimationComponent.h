#pragma once

#include "Sprite.h"

namespace dd
{
	struct SpriteAnimationComponent
	{
		dd::Vector<ddr::SpriteHandle> Frames;
		int CurrentFrame { 0 };
		int Framerate { 30 };
		float Time { 0 };
		bool IsPlaying { false };

		void PlayFromStart()
		{
			CurrentFrame = 0;
			Time = 0;
			IsPlaying = true;
		}

		void Stop()
		{
			CurrentFrame = 0;
			Time = 0;
			IsPlaying = false;
		}

		DD_BEGIN_CLASS(SpriteAnimationComponent)
			DD_COMPONENT();

			DD_MEMBER(Frames);
			DD_MEMBER(CurrentFrame);
			DD_MEMBER(Framerate);
			DD_MEMBER(Time);
		DD_END_CLASS()
	};
}