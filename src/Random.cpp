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
	Random64::Random64( uint64 seed ) :
		m_rng( seed ),
		m_min( std::numeric_limits<uint64>::min() ),
		m_max( std::numeric_limits<uint64>::max() )
	{
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	Random64::Random64( uint64 min, uint64 max, uint64 seed )
		: m_min( min ),
		m_max( max ),
		m_rng( seed )
	{
		DD_ASSERT( min < max );

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

	Random32::Random32( uint seed ) :
		m_rng( seed ),
		m_min( std::numeric_limits<uint>::min() ),
		m_max( std::numeric_limits<uint>::max() )
	{
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}
	
	Random32::Random32( uint min, uint max, uint seed )
		: m_min( min ),
		m_max( max ),
		m_rng( seed )
	{
		DD_ASSERT( min < max );

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

	RandomInt::RandomInt( int seed ) :
		m_rng( seed ),
		m_min( std::numeric_limits<int>::min() ),
		m_max( std::numeric_limits<int>::max() )
	{
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	RandomInt::RandomInt( int min, int max, int seed ) :
		m_rng( seed ),
		m_min( min ),
		m_max( max )
	{
		DD_ASSERT( min < max );

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

	RandomFloat::RandomFloat( uint seed ) :
		m_rng( seed ),
		m_min( 0.0f ),
		m_max( 1.0f )
	{
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	RandomFloat::RandomFloat( float min, float max, uint seed ) :
		m_rng( seed ),
		m_min( min ),
		m_max( max )
	{
		DD_ASSERT( min < max );
		
		if( seed == DEFAULT_SEED )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	float RandomFloat::Next()
	{
		uint val = m_rng();

		float rem = m_min + (val / static_cast<float>(UINT_MAX / (m_max - m_min)));

		return rem;
	}
}