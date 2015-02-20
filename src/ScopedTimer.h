//
// ScopedTimer.h - Scoped timer class to easily time operations.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#pragma once

#include <ctime>

class ScopedTimer
{
public:
	ScopedTimer( float& elapsed_ms )
		: m_result( elapsed_ms )
	{
		m_start = std::clock();
	}

	~ScopedTimer()
	{
		std::clock_t duration = std::clock() - m_start;

		m_result = duration / (float) CLOCKS_PER_SEC;
	}


private:

	float& m_result;
	std::clock_t m_start;
};