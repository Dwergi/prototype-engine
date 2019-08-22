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
	int dd::Profiler::s_frameCount = 0;

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
		int frames = ddm::min(dd::Profiler::FrameCount(), FRAME_COUNT);
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
		size_t group_end = m_name.rfind('/');
		if (group_end == std::string::npos)
		{
			group_end = 0;
		}
		else
		{
			++group_end;
		}

		if (ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_Framed, "%s: %.2f", m_name.substr(group_end).c_str(), GetValue()))
		{
			int frames = ddm::min(dd::Profiler::FrameCount(), FRAME_COUNT);

			ImGui::PlotLines("", &ProfilerValueGetter, this, frames - 1, 0, nullptr, 0, 50, ImVec2(200, 50));
			ImGui::Value("Average Over 100", m_sliding);

			ImGui::TreePop();
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

		// insert alphabetically
		size_t i = 0;
		while (i < s_instances.size() && s_instances[i]->Name() < name)
		{
			++i;
		}
		s_instances.insert(s_instances.begin() + i, value);

		return *value;
	}

	int Profiler::FrameCount()
	{
		return s_frameCount;
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

		++s_frameCount;
	}

	static Array<std::string, 4> s_currentGroups;
	static int s_openGroups = 0;

	static bool CreateGroups(const std::string& name)
	{
		dd::Array<std::string, 4> groups;

		// enumerate groups
		size_t start = 0;
		size_t end = name.find('/');
		while (end != std::string::npos)
		{
			std::string group_name = name.substr(start, end - start);
			groups.Add(group_name);

			start = end + 1;
			end = name.find('/', end + 1);
		}

		// count matches
		int matches = 0;
		for (int i = 0; i < groups.Size() && i < s_currentGroups.Size(); ++i)
		{
			if (groups[i] == s_currentGroups[i])
			{
				++matches;
			}
		}

		s_currentGroups = groups;

		// pop groups
		int to_pop = s_openGroups - matches;
		for (int i = 0; i < to_pop; ++i)
		{
			ImGui::TreePop();
			--s_openGroups;
		}

		// push groups
		for (int i = matches; i < s_currentGroups.Size() && i <= s_openGroups; ++i)
		{
			if (ImGui::TreeNodeEx(s_currentGroups[i].c_str(), ImGuiTreeNodeFlags_Framed))
			{
				++s_openGroups;
			}
			else
			{
				break;
			}
		}
		
		return s_currentGroups.Size() == s_openGroups;
	}

	void Profiler::Draw()
	{
		DD_ASSERT(dd::IsMainThread());

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
				if (CreateGroups(value->Name()))
				{
					value->Draw();
				}
			}

			for (int i = 0; i < s_openGroups; ++i)
			{
				ImGui::TreePop();
				--s_openGroups;
			}

			s_currentGroups.Clear();

			ImGui::End();
		}
	}

	void Profiler::EnableDraw(bool draw)
	{
		s_draw = draw;
	}
}

