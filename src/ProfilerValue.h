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

	struct IProfilerValue
	{
		virtual void Increment() = 0;
		virtual void Decrement() = 0;

		virtual void SetValue(float value) = 0;
		virtual float GetValue() const = 0;
		virtual float GetValueAtIndex(int index) const = 0;

		virtual int Index() const = 0;
		virtual float SlidingAverage() const = 0;

		virtual std::string_view Name() const = 0;
		virtual std::string_view FullName() const = 0;
		virtual const dd::IArray<std::string>& Groups() const = 0;
	};

	struct ProfilerValue final : IProfilerValue
	{
		static const int FRAME_COUNT = 100;

		ProfilerValue(const ProfilerValue&) = delete;
		ProfilerValue(ProfilerValue&&) = delete;

		void Increment() override;
		void Decrement() override;

		void SetValue(float value) override;
		float GetValue() const override;
		float GetValueAtIndex(int index) const override;

		int Index() const override { return m_index; }
		float SlidingAverage() const override { return m_sliding; }

		std::string_view Name() const override { return m_name; }
		std::string_view FullName() const override { return m_fullName; }

		const dd::IArray<std::string>& Groups() const override  { return m_groups; }

	private:
		friend struct Profiler;

		dd::Array<std::string, 4> m_groups;
		std::string m_name;
		std::string m_fullName;
		int m_index { 0 };
		float m_sliding { 0 };
		float m_values[FRAME_COUNT] = { 0 };

		ProfilerValue(std::string_view name);

		void BeginFrame();
		void EndFrame();
		void Draw();
	};

	struct ProfilerValueRef final : IProfilerValue
	{
		ProfilerValueRef(std::string_view full_name);

		void Increment() override;
		void Decrement() override;

		void SetValue(float value) override;
		float GetValue() const override;
		float GetValueAtIndex(int index) const override;

		int Index() const override;
		float SlidingAverage() const override;

		std::string_view Name() const override;
		std::string_view FullName() const override { return m_fullName; }
		const dd::IArray<std::string>& Groups() const override;

	private:
		std::string m_fullName;
		ProfilerValue* m_ptr { 0 };

		void Initialize();
	};
}