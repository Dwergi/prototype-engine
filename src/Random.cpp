//
// Random.cpp - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Random.h"

#include <random>
#include <ctime>

#include "pcg/pcg_extras.hpp"

namespace dd
{
	Random64::Random64( uint64 min, uint64 max, uint64 seed )
		: m_min( min ),
		m_max( max ),
		m_rng( seed )
	{
		ASSERT( min < max );

		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	uint64 Random64::Next()
	{
		uint64 out = m_rng();

		uint64 clamped = m_min + out % (m_max - m_min);

		return clamped;
	}

	Random32::Random32( uint min, uint max, uint seed )
		: m_min( min ),
		m_max( max ),
		m_rng( seed )
	{
		ASSERT( min < max );

		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	uint Random32::Next()
	{
		uint out = m_rng();

		uint clamped = m_min + out % (m_max - m_min);

		return clamped;
	}

	RandomInt::RandomInt( int min, int max, int seed )
		: m_min( min ),
		m_max( max ),
		m_rng( seed )
	{
		ASSERT( min < max );

		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	int RandomInt::Next()
	{
		uint out = m_rng();

		int clamped = m_min + out % (m_max - m_min);

		return clamped;
	}
}