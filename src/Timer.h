//
// Timer.h - Timer base implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <ctime>

namespace dd
{
	class Timer
	{
	public:
		Timer();
		void Start();
		void Pause();
		float Stop();
		float Time() const;

	private:

		std::time_t m_elapsed;
		std::time_t m_start;
	};
}