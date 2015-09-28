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
	Random::Random( uint min, uint max, uint seed )
	{
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	int Random::Next()
	{
		uint64_t out = m_rng();

		return (int) out;
	}
}