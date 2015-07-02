#include "PrecompiledHeader.h"
#include "Timer.h"

dd::Timer::Timer()
	: m_elapsed( 0 ),
	m_start( 0 )
{

}

void dd::Timer::start()
{
	ASSERT( m_start == 0 );

	m_start = std::clock();
}

void dd::Timer::pause()
{
	ASSERT( m_start > 0 );

	std::time_t current = std::clock();
	m_elapsed += current - m_start;

	m_start = 0;
}

float dd::Timer::stop()
{
	ASSERT( m_start > 0 );

	pause();
	return time();
}

float dd::Timer::time()
{
	if( m_start == 0 )
		return 0;

	std::time_t current = std::clock();
	return (m_elapsed + (current - m_start)) / (float) CLOCKS_PER_SEC;
}