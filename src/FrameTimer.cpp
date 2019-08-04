//
// FrameTimer.cpp - Frame timer that keeps around a sliding window of FPS.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#include "PCH.h"
#include "FrameTimer.h"

namespace dd
{
	FrameTimer::FrameTimer() : 
		m_maxFPS(120),
		m_targetDelta(1.0f / m_maxFPS),
		m_frameTimes( "Frame Time", m_targetDelta)
	{
		m_lastFrameTime = 0.0f;
		m_currentFrameTime = -m_targetDelta;
		m_gameDelta = m_targetDelta;
		m_appDelta = m_targetDelta;
		m_deltaWithoutDelay = m_targetDelta;

		m_timer.Start();
	}

	void FrameTimer::Update()
	{
		++m_frame;

		m_targetDelta = 1.0f / m_maxFPS;

		m_lastFrameTime = m_currentFrameTime;
		m_currentFrameTime = (float) m_timer.Time();
		
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

		m_frameTimes.BeginFrame();
		m_frameTimes.SetValue( m_deltaWithoutDelay * 1000.f );
	}

	void FrameTimer::DelayFrame()
	{
		DD_PROFILE_SCOPED( FrameTimer_DelayFrame );

		float now = (float) m_timer.Time();
		float delta_t = (now - m_lastFrameTime);

		m_deltaWithoutDelay = delta_t;

		while( delta_t < m_targetDelta )
		{
			float remainder_t = m_targetDelta - delta_t;
			std::this_thread::sleep_for( std::chrono::milliseconds( (int) remainder_t * 1000 ) );

			delta_t = (float) m_timer.Time() - m_lastFrameTime;
		}

		m_frameTimes.EndFrame();
	}

	void FrameTimer::DrawDebugInternal( ddc::World& world )
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, 30.0f ), ImGuiCond_FirstUseEver );

		ImGui::Checkbox( "Compact Counter", &m_drawCompact );
		ImGui::SliderFloat( "Time Scale", &m_timeScale, 0.0f, 4.0f, "%.3f", 2.0f );

		ImGui::Value( "FPS: ", 1000.f / m_frameTimes.SlidingAverage(), "%.1f" );

		m_frameTimes.Draw();
	}

	void FrameTimer::DrawFPSCounter()
	{
		if( m_drawCompact )
		{
			ImGui::SetNextWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiCond_Always );
			ImGui::SetNextWindowSize( ImVec2( 45, 20 ), ImGuiCond_Always );

			ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 0.5 ) );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_ItemInnerSpacing, ImVec2( 0, 0 ) );

			ImGui::Begin( "CompactFPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs );
			ImGui::Text( "%.1f", 1000.f / m_frameTimes.SlidingAverage() );
			ImGui::SameLine();
			ImGui::End();

			ImGui::PopStyleVar( 2 );
			ImGui::PopStyleColor();
		}
	}
}