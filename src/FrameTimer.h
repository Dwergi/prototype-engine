//
// FrameTimer.h - Frame timer that keeps around a sliding window of FPS.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "IDebugDraw.h"
#include "Timer.h"

namespace dd
{
	class FrameTimer : public IDebugDraw
	{
	public:

		FrameTimer();

		void Update();
		void DelayFrame();

		void SetMaxFPS( uint max_fps )
		{
			m_maxFPS = max_fps;
		}

		float Delta() const
		{
			return m_delta;
		}

		float SlidingDelta() const
		{
			return m_slidingDelta;
		}

		float DeltaWithoutDelay() const
		{
			return m_deltaWithoutDelay;
		}

		virtual const char* GetDebugTitle() const override
		{
			return "FPS";
		}

		void DrawFPSCounter();

	protected:

		virtual void DrawDebugInternal() override;

	private:

		Timer m_timer;
		float m_targetDelta;
		float m_lastFrameTime;
		float m_currentFrameTime;
		float m_delta;
		float m_deltaWithoutDelay;
		float m_slidingDelta;
		uint m_maxFPS;
		bool m_drawCompact { true };

		static const int SLIDING_WINDOW_SIZE = 60;
		float m_frameTimes[SLIDING_WINDOW_SIZE];
		int m_currentSlidingFrame = 0;
	};
}