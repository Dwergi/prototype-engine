//
// Timer.cpp -  Timer base implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "Timer.h"

namespace dd
{
	// Jesus fucking christ this API is atrocious.
	using clock_t = std::chrono::high_resolution_clock;
	using time_point_t = std::chrono::high_resolution_clock::time_point;
	using microsec_t = std::chrono::microseconds;

	constexpr time_point_t ZERO_TIME = time_point_t(microsec_t(0));

	Timer::Timer()
	{
		m_start = ZERO_TIME;
		m_elapsed = microsec_t(0);
	}

	void Timer::Start()
	{
		DD_ASSERT(m_start == ZERO_TIME);

		m_start = clock_t::now();
	}

	void Timer::Restart()
	{
		m_start = ZERO_TIME;
		m_elapsed = microsec_t(0);
		Start();
	}

	void Timer::Pause()
	{
		DD_ASSERT(m_start != ZERO_TIME);

		time_point_t current = clock_t::now();
		m_elapsed += std::chrono::duration_cast<microsec_t>(current - m_start);

		m_start = ZERO_TIME;
	}

	float Timer::Stop()
	{
		DD_ASSERT(m_start != ZERO_TIME);

		float time = TimeInSeconds();
		m_elapsed = microsec_t(0);
		m_start = ZERO_TIME;

		return time;
	}

	float Timer::TimeInSeconds() const
	{
		if (m_start == ZERO_TIME)
		{
			return 0;
		}

		constexpr float MICROSECS_PER_SEC = 1000.0f * 1000.0f;

		time_point_t current = clock_t::now();
		microsec_t microseconds = std::chrono::duration_cast<microsec_t>(current - m_start);
		microsec_t total = m_elapsed + microseconds;

		return total.count() / MICROSECS_PER_SEC;
	}

	float Timer::TimeInMilliseconds() const
	{
		if (m_start == ZERO_TIME)
		{
			return 0;
		}

		constexpr float MICROSECS_PER_MILLISEC = 1000.0f;

		time_point_t current = clock_t::now();
		microsec_t microseconds = std::chrono::duration_cast<microsec_t>(current - m_start);
		microsec_t total = m_elapsed + microseconds;

		return total.count() / MICROSECS_PER_MILLISEC;
	}
}