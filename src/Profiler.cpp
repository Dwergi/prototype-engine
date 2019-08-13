//
// Profiler.cpp
// Copyright (C) Sebastian Nordgren 
// January 27th 2019
//

#include "PCH.h"
#include "Profiler.h"

#include "IWindow.h"

#include <fmt/format.h>

namespace dd
{
	std::vector<dd::ProfilerValue*> dd::Profiler::s_instances;
	bool dd::Profiler::s_draw = false;
	bool dd::Profiler::s_inFrame = false;

	static dd::Service<dd::IWindow> s_window;

	ProfilerValue::ProfilerValue(const char* name) :
		m_name(name)
	{
		m_sliding = 0;
	}

	void ProfilerValue::Increment()
	{
		++m_values[m_index];
	}

	void ProfilerValue::SetValue(float value)
	{
		m_values[m_index] = value;
	}

	float ProfilerValue::GetValue() const
	{
		return m_values[m_index];
	}

	float ProfilerValue::GetValueAtIndex(int index) const
	{
		DD_ASSERT(index >= 0 && index < FRAME_COUNT);

		return m_values[index];
	}

	void ProfilerValue::BeginFrame()
	{
		++m_frameCount;
		++m_index;

		if (m_index >= FRAME_COUNT)
		{
			m_index = 0;
		}

		m_values[m_index] = 0;
	}

	void ProfilerValue::EndFrame()
	{
		float total = 0;
		int frames = ddm::min(m_frameCount, FRAME_COUNT);
		for (int i = 0; i < frames; ++i)
		{
			total += m_values[i];
		}

		m_sliding = total / frames;
	}

	float ProfilerValueGetter(void* data, int index)
	{
		ProfilerValue* value = (ProfilerValue*) data;

		int actual = value->Index() + index + 1;
		if (actual >= ProfilerValue::FRAME_COUNT)
		{
			actual -= ProfilerValue::FRAME_COUNT;
		}

		float f = value->GetValueAtIndex(actual);
		return f;
	}

	void ProfilerValue::Draw()
	{
		if (ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_CollapsingHeader, "%s: %.2f", m_name.c_str(), GetValue()))
		{
			int frames = ddm::min(m_frameCount, FRAME_COUNT);

			ImGui::PlotLines("", &ProfilerValueGetter, this, frames - 1, 0, nullptr, 0, 50, ImVec2(200, 50));
			ImGui::Value("Average Over 100", m_sliding);
		}
	}

	ProfilerValue& Profiler::GetValue(const char* name)
	{
		for (dd::ProfilerValue* value : s_instances)
		{
			if (value->Name() == name)
			{
				return *value;
			}
		}

		ProfilerValue* value = new ProfilerValue(name);
		s_instances.push_back(value);
		return *value;
	}


	void Profiler::BeginFrame()
	{
		DD_ASSERT(!s_inFrame);
		s_inFrame = true;

		for (dd::ProfilerValue* value : s_instances)
		{
			value->BeginFrame();
		}
	}

	void Profiler::EndFrame()
	{
		DD_ASSERT(s_inFrame);
		s_inFrame = false;

		for (dd::ProfilerValue* value : s_instances)
		{
			value->EndFrame();
		}
	}

	void Profiler::Draw()
	{
		if (!s_draw)
		{
			return;
		}

		glm::ivec2 window_size = s_window->GetSize();
		ImGui::SetNextWindowPos(ImVec2(window_size.x - 200.f, 30), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Profiler", &s_draw, ImVec2(170, 170), 0.4f))
		{
			for (dd::ProfilerValue* value : s_instances)
			{
				value->Draw();
			}

			ImGui::End();
		}
	}

	void Profiler::EnableDraw(bool draw)
	{
		s_draw = draw;
	}
}

