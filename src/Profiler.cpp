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
	Remotery* dd::Profiler::s_remotery = nullptr;
	std::vector<dd::ProfilerValue*> dd::Profiler::s_instances;
	bool dd::Profiler::s_draw = false;
	bool dd::Profiler::s_inFrame = false;
	int dd::Profiler::s_frameCount = 0;

	static dd::Service<dd::IWindow> s_window;

	void Profiler::Initialize()
	{
		if (s_remotery == nullptr)
		{
			DD_PROFILE_INIT(s_remotery);
			DD_PROFILE_THREAD_NAME("Main");
		}
	}

	void Profiler::Shutdown()
	{
		DD_ASSERT(s_remotery != nullptr);

		DD_PROFILE_DEINIT(s_remotery);
	}

	ProfilerValue& Profiler::GetValue(std::string_view full_name)
	{
		Initialize();

		for (dd::ProfilerValue* value : s_instances)
		{
			if (value->FullName() == full_name)
			{
				return *value;
			}
		}

		ProfilerValue* value = new ProfilerValue(full_name);

		// insert alphabetically
		size_t i = 0;
		for (const dd::ProfilerValue* value : s_instances)
		{
			const int cmp = full_name.compare(value->FullName());
			if (cmp < 0)
			{
				break;
			}

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
		ImGui::SetNextWindowSize(ImVec2(170, 170), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowBgAlpha(0.4f);

		if (ImGui::Begin("Profiler", &s_draw))
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
