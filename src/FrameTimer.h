//
// FrameTimer.h - Frame timer that keeps around a sliding window of FPS.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "IDebugPanel.h"
#include "Timer.h"

namespace dd
{
	class FrameTimer : public IDebugPanel
	{
	public:

		FrameTimer();

		void Update();
		void DelayFrame();

		void SetPaused( bool paused ) { m_paused = paused; }
		bool IsPaused() const { return m_paused; }

		void SetTimeScale( float scale ) { m_timeScale = scale; }
		float GetTimeScale() const { return m_timeScale; }

		void SetMaxFPS( uint max_fps ) { m_maxFPS = max_fps; }
		uint GetMaxFPS() const { return m_maxFPS; }

		float GameDelta() const { return m_gameDelta; }
		float AppDelta() const { return m_appDelta; }

		float SlidingDelta() const { return m_slidingDelta; }
		float DeltaWithoutDelay() const { return m_deltaWithoutDelay; }

		virtual const char* GetDebugTitle() const override
		{
			return "FPS";
		}

		void DrawFPSCounter();

	protected:

		virtual void DrawDebugInternal( ddc::World& world ) override;

	private:

		Timer m_timer;
		float m_targetDelta { 0.0f };
		float m_lastFrameTime { 0.0f };
		float m_currentFrameTime { 0.0f };
		float m_gameDelta { 0.0f };
		float m_appDelta { 0.0f };
		float m_deltaWithoutDelay { 0.0f };
		float m_slidingDelta { 0.0f };
		float m_timeScale { 1.0f };
		uint m_maxFPS { 0 };
		bool m_drawCompact { true };
		bool m_paused { false };

		static const int SLIDING_WINDOW_SIZE = 60;
		float m_frameTimes[SLIDING_WINDOW_SIZE];
		int m_currentSlidingFrame = 0;
	};
}