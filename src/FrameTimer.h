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
	struct FrameTimer : public IDebugPanel
	{
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

		float SlidingDelta() const;
		float DeltaWithoutDelay() const { return m_deltaWithoutDelay; }

		virtual const char* GetDebugTitle() const override { return "FPS"; }

		void DrawFPSCounter();

	private:

		Timer m_timer;
		uint m_maxFPS { 0 };
		float m_targetDelta { 0.0f };
		float m_lastFrameTime { 0.0f };
		float m_currentFrameTime { 0.0f };
		float m_gameDelta { 0.0f };
		float m_appDelta { 0.0f };
		float m_deltaWithoutDelay { 0.0f };
		float m_timeScale { 1.0f };
		bool m_drawCompact { true };
		bool m_paused { false };
		uint m_frame { 0 };

		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;
	};
}