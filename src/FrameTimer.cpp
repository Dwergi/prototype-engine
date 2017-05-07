//
// FrameTimer.cpp - Frame timer that keeps around a sliding window of FPS.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#include "PrecompiledHeader.h"
#include "FrameTimer.h"

#include "imgui/imgui.h"

#include <thread>

namespace dd
{
	FrameTimer::FrameTimer()
	{
		m_maxFPS = 120;
		m_targetDelta = 1.0f / m_maxFPS;
		m_lastFrameTime = 0.0f;
		m_currentFrameTime = -m_targetDelta;
		m_delta = m_targetDelta;
		m_deltaWithoutDelay = m_targetDelta;

		// fill history with standard deltas
		for( int i = 0; i < SLIDING_WINDOW_SIZE; ++i )
		{
			m_frameTimes[i] = m_targetDelta * 1000.0f;
		}

		m_timer.Start();
	}

	void FrameTimer::Update()
	{
		m_targetDelta = 1.0f / m_maxFPS;
		m_lastFrameTime = m_currentFrameTime;
		m_currentFrameTime = m_timer.Time();
		m_delta = m_currentFrameTime - m_lastFrameTime;

		// update sliding window
		m_frameTimes[m_currentSlidingFrame] = m_deltaWithoutDelay * 1000.f;
		++m_currentSlidingFrame;

		if( m_currentSlidingFrame >= SLIDING_WINDOW_SIZE )
			m_currentSlidingFrame = 0;

		float total_time = 0;
		for( float f : m_frameTimes )
		{
			total_time += f;
		}

		m_slidingDelta = (total_time / SLIDING_WINDOW_SIZE) / 1000.f;
	}

	void FrameTimer::DelayFrame()
	{
		DD_PROFILE_SCOPED( FrameTimer_DelayFrame );

		float now = m_timer.Time();
		float delta_t = (now - m_lastFrameTime);

		m_deltaWithoutDelay = delta_t;

		while( delta_t < m_targetDelta )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( (int) delta_t * 1000 ) );

			delta_t = m_timer.Time() - m_lastFrameTime;
		}
	}

	void FrameTimer::DrawDebugInternal()
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, 30.0f ), ImGuiSetCond_FirstUseEver );
		ImGui::SetWindowSize( ImVec2( 250.0f, 100.0f ), ImGuiSetCond_FirstUseEver );

		ImGui::Text( "FPS: %.1f", 1.0f / m_slidingDelta );
		ImGui::Text( "Frame Time: %.1f", m_deltaWithoutDelay * 1000.f );
		ImGui::Text( "Sliding: %.1f", m_slidingDelta );
		
		if( ImGui::TreeNodeEx( "Frame Times", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::PlotLines( "", m_frameTimes, SLIDING_WINDOW_SIZE, 0, nullptr, 0, 50, ImVec2( 200, 50 ) );
			ImGui::TreePop();
		}
	}
}