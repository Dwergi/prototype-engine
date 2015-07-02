//
// ScopedTimer.h - Scoped timer class to easily time operations.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#pragma once

#include <ctime>

namespace dd
{
	class ScopedTimer
	{
	public:
		ScopedTimer( float& elapsed_ms );
		~ScopedTimer();

	private:

		float& m_result;
		std::clock_t m_start;
	};
}