//
// Profiler.cpp
// Copyright (C) Sebastian Nordgren 
// August 23rd 2019
//

#include "PCH.h"
#include "ProfilerValue.h"

#include <fmt/format.h>

namespace dd
{
	ProfilerValue::ProfilerValue(std::string_view name)
	{
		std::string str_name(name);

		// enumerate groups
		size_t start = 0;
		size_t end = str_name.find('/');
		while (end != std::string::npos)
		{
			std::string group_name = str_name.substr(start, end - start);
			m_groups.Add(group_name);

			start = end + 1;
			end = str_name.find('/', end + 1);
		}

		size_t group_end = str_name.rfind('/');
		if (group_end == std::string::npos)
		{
			group_end = 0;
		}
		else
		{
			group_end += 1;
		}

		m_name = str_name.substr(group_end);
		m_sliding = 0;
	}

	void ProfilerValue::Increment()
	{
		++m_values[m_index];
	}

	void ProfilerValue::Decrement()
	{
		--m_values[m_index];
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

		++m_index;
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
		char buffer[64];
		std::snprintf(buffer, 64, "%s: %.2f", m_name.c_str(), GetValue());

		if (ImGui::TreeNodeEx(this, ImGuiTreeNodeFlags_Framed, buffer))
		{
			int frames = ddm::min(dd::Profiler::FrameCount(), FRAME_COUNT);

			ImGui::PlotLines("", &ProfilerValueGetter, this, frames - 1, 0, nullptr, 0, 50, ImVec2(200, 50));
			ImGui::Value("Average Over 100", m_sliding);

			ImGui::TreePop();
		}
	}

	ProfilerValueRef::ProfilerValueRef(std::string_view name) :
		m_name(name)
	{
	}

	void ProfilerValueRef::Initialize()
	{
		if (m_ptr == nullptr)
		{
			m_ptr = &dd::Profiler::GetValue(m_name);
		}
	}

	void ProfilerValueRef::Increment()
	{
		Initialize();
		m_ptr->Increment();
	}

	void ProfilerValueRef::Decrement()
	{
		Initialize();
		m_ptr->Decrement();
	}

	void ProfilerValueRef::SetValue(float value)
	{
		Initialize();
		m_ptr->SetValue(value);
	}

	float ProfilerValueRef::GetValue() const
	{
		DD_ASSERT(m_ptr != nullptr);
		return m_ptr->GetValue();
	}

	float ProfilerValueRef::GetValueAtIndex(int index) const
	{
		DD_ASSERT(m_ptr != nullptr);
		return m_ptr->GetValueAtIndex(index);
	}

	int ProfilerValueRef::Index() const
	{
		DD_ASSERT(m_ptr != nullptr);
		return m_ptr->Index();
	}

	float ProfilerValueRef::SlidingAverage() const
	{
		DD_ASSERT(m_ptr != nullptr);
		return m_ptr->SlidingAverage();
	}

	const dd::IArray<std::string>& ProfilerValueRef::Groups() const
	{
		DD_ASSERT(m_ptr != nullptr);
		return m_ptr->Groups();
	}
}