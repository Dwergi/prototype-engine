//
// ProfilerValue.h - Macros to use with profiler.
// Copyright (C) Sebastian Nordgren 
// August 23rd 2019
//

#pragma once

#include "Array.h"

namespace dd
{
	struct Profiler;

	struct ProfilerValue
	{
		static const int FRAME_COUNT = 100;

		ProfilerValue(const ProfilerValue&) = delete;
		ProfilerValue(ProfilerValue&&) = delete;

		void Increment();

		void SetValue(float value);
		float GetValue() const;
		float GetValueAtIndex(int index) const;

		int Index() const { return m_index; }
		float SlidingAverage() const { return m_sliding; }

		const std::string& Name() const { return m_name; }
		const dd::IArray<std::string>& Groups() const { return m_groups; }

	private:
		friend struct Profiler;

		dd::Array<std::string, 4> m_groups;
		std::string m_name;
		int m_index { 0 };
		float m_sliding { 0 };
		float m_values[FRAME_COUNT] = { 0 };

		ProfilerValue(const char* name);

		void BeginFrame();
		void EndFrame();
		void Draw();
	};
}