#include "PrecompiledHeader.h"
#include "ScopedTimer.h"

dd::ScopedTimer::ScopedTimer( float& elapsed_ms )
	: m_result( elapsed_ms )
{
	m_start = std::clock();
}

dd::ScopedTimer::~ScopedTimer()
{
	std::clock_t duration = std::clock() - m_start;

	m_result = duration / (float) CLOCKS_PER_SEC;
}