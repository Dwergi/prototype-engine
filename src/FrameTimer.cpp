//
// FrameTimer.cpp - Frame timer that keeps around a sliding window of FPS.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#include "PCH.h"
#include "FrameTimer.h"



#include <thread>

namespace dd
{
	FrameTimer::FrameTimer()
	{
		m_maxFPS = 120;
		m_targetDelta = 1.0f / m_maxFPS;
		m_lastFrameTime = 0.0f;
		m_currentFrameTime = -m_targetDelta;
		m_gameDelta = m_targetDelta;
		m_appDelta = m_targetDelta;
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
		++m_frame;

		m_targetDelta = 1.0f / m_maxFPS;

		m_lastFrameTime = m_currentFrameTime;
		m_currentFrameTime = m_timer.Time();
		
		m_appDelta = m_currentFrameTime - m_lastFrameTime;

		if( m_appDelta > 1.0f && dd::DebuggerAttached() )
		{
			m_appDelta = m_targetDelta;
		}

		if( !m_paused )
		{
			m_gameDelta = m_appDelta * m_timeScale;
		}
		else
		{
			m_gameDelta = 0;
		}

		// update sliding window
		m_frameTimes[m_currentSlidingFrame] = m_deltaWithoutDelay * 1000.f;
		++m_currentSlidingFrame;

		if( m_currentSlidingFrame >= SLIDING_WINDOW_SIZE )
		{
			m_currentSlidingFrame = 0;
		}

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

	void FrameTimer::DrawDebugInternal( ddc::World& world )
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, 30.0f ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Compact Counter", &m_drawCompact );

		ImGui::Value( "FPS: ", 1.0f / m_slidingDelta, "%.1f" );
		ImGui::Text( "Frame Time: %.1fms", m_deltaWithoutDelay * 1000.f );
		ImGui::Text( "Sliding Time: %.1fms", m_slidingDelta * 1000.0f );

		ImGui::SliderFloat( "Time Scale", &m_timeScale, 0.0f, 4.0f, "%.3f", 2.0f );
		
		if( ImGui::TreeNodeEx( "Frame Times", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::PlotLines( "", m_frameTimes, SLIDING_WINDOW_SIZE, 0, nullptr, 0, 50, ImVec2( 200, 50 ) );
			ImGui::TreePop();
		}
	}

	void FrameTimer::DrawFPSCounter()
	{
		if( m_drawCompact )
		{
			ImGui::SetNextWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiSetCond_Always );
			ImGui::SetNextWindowSize( ImVec2( 45, 20 ), ImGuiSetCond_Always );

			ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0.5 ) );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_ItemInnerSpacing, ImVec2( 0, 0 ) );

			ImGui::Begin( "CompactFPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs );
			ImGui::Text( "%.1f", 1.0f / m_slidingDelta );
			ImGui::SameLine();
			ImGui::End();

			ImGui::PopStyleVar( 2 );
			ImGui::PopStyleColor();
		}
	}
}