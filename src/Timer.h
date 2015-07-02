#pragma once

#include <ctime>

namespace dd
{
	class Timer
	{
	public:
		Timer();
		void start();
		void pause();
		float stop();
		float time();

	private:

		std::time_t m_elapsed;
		std::time_t m_start;
	};
}