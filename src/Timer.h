//
// Timer.h - Timer base implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <chrono>

namespace dd
{
	class Timer
	{
	public:
		Timer();
		void Start();
		void Restart();
		void Pause();
		float Stop();
		float TimeInSeconds() const;
		float TimeInMilliseconds() const;

	private:

		std::chrono::microseconds m_elapsed;
		std::chrono::high_resolution_clock::time_point m_start;
	};
}