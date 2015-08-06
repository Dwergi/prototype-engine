//
// Timer.cpp -  Timer base implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Timer.h"

dd::Timer::Timer()
	: m_elapsed( 0 ),
	m_start( 0 )
{

}

void dd::Timer::Start()
{
	ASSERT( m_start == 0 );

	m_start = std::clock();
}

void dd::Timer::Pause()
{
	ASSERT( m_start > 0 );

	std::time_t current = std::clock();
	m_elapsed += current - m_start;

	m_start = 0;
}

float dd::Timer::Stop()
{
	ASSERT( m_start > 0 );

	Pause();
	return Time();
}

float dd::Timer::Time() const
{
	if( m_start == 0 )
		return 0;

	std::time_t current = std::clock();
	return (m_elapsed + (current - m_start)) / (float) CLOCKS_PER_SEC;
}