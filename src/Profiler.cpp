//
// Profiler.cpp
// Copyright (C) Sebastian Nordgren 
// January 27th 2019
//

#include "PCH.h"
#include "Profiler.h"

#include "IWindow.h"

namespace dd
{
	std::vector<dd::ProfilerValue*> dd::Profiler::s_instances;
	bool dd::Profiler::s_draw = false;
	bool dd::Profiler::s_inFrame = false;
	int dd::Profiler::s_frameCount = 0;

	static dd::Service<dd::IWindow> s_window;

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

	static bool CreateGroups(const dd::IArray<std::string>& groups)
	{
		// count matches
		int matches = 0;
		for (int i = 0; i < groups.Size() && i < s_currentGroups.Size(); ++i)
		{
			if (groups[i] == s_currentGroups[i])
			{
				++matches;
			}
		}

		s_currentGroups.Clear();
		s_currentGroups.PushAll(groups);

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
				if (CreateGroups(value->Groups()))
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

