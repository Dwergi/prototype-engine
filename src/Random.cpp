//
// Random.cpp - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
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
		if( seed == DEFAULT_SEED_64 )
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

		if( seed == DEFAULT_SEED_64 )
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

	Random32::Random32( uint seed ) : Random32Base<uint>( seed ) {}
	Random32::Random32( uint min, uint max, uint seed ) : Random32Base<uint>( min, max, seed ) {}

	uint Random32::Next()
	{
		DD_ASSERT( m_min < m_max );

		uint out = m_rng();

		uint clamped = m_min + out % (m_max - m_min);
		return clamped;
	}


	RandomInt::RandomInt( uint seed ) : Random32Base<int>( seed ) {}
	RandomInt::RandomInt( int min, int max, uint seed ) : Random32Base<int>( min, max, seed ) {}

	int RandomInt::Next()
	{
		DD_ASSERT( m_min < m_max );

		uint out = m_rng();

		int clamped = m_min + out % (m_max - m_min);
		return clamped;
	}

	RandomFloat::RandomFloat( uint seed ) : Random32Base<float>( 0, 1, seed ) {}
	RandomFloat::RandomFloat( float min, float max, uint seed ) : Random32Base<float>( min, max, seed ) {}

	float RandomFloat::Next()
	{
		DD_ASSERT( m_min < m_max );

		uint val = m_rng();
		
		float factor = val / (float) UINT_MAX; // between 0 and 1
		float fval = m_min + (m_max - m_min) * factor;

		return fval;
	}

	RandomVector3::RandomVector3( uint seed ) :
		m_rngX( seed ),
		m_rngY( seed ),
		m_rngZ( seed )
	{
	}

	RandomVector3::RandomVector3( glm::vec3 min, glm::vec3 max, uint seed ) :
		m_rngX( min.x, max.y, seed ),
		m_rngY( min.y, max.y, seed ),
		m_rngZ( min.z, max.z, seed )
	{
	}

	glm::vec3 RandomVector3::Min() const
	{
		return glm::vec3( m_rngX.Min(), m_rngY.Min(), m_rngZ.Min() );
	}

	void RandomVector3::SetMin( glm::vec3 min )
	{
		m_rngX.SetMin( min.x );
		m_rngY.SetMin( min.y );
		m_rngZ.SetMin( min.z );
	}

	glm::vec3 RandomVector3::Max() const
	{
		return glm::vec3( m_rngX.Max(), m_rngY.Max(), m_rngZ.Max() );
	}

	void RandomVector3::SetMax( glm::vec3 max )
	{
		m_rngX.SetMax( max.x );
		m_rngY.SetMax( max.y );
		m_rngZ.SetMax( max.z );
	}

	glm::vec3 RandomVector3::Next()
	{
		glm::vec3 value;
		value.x = m_rngX.Next();
		value.y = m_rngY.Next();
		value.z = m_rngZ.Next();

		return value;
	}

	glm::vec2 GetRandomVector2( dd::RandomFloat& rng, glm::vec2 min, glm::vec2 max )
	{
		return glm::mix( min, max, glm::vec2( rng.Next(), rng.Next() ) );
	}

	glm::vec3 GetRandomVector3( dd::RandomFloat& rng, glm::vec3 min, glm::vec3 max )
	{
		return glm::mix( min, max, glm::vec3( rng.Next(), rng.Next(), rng.Next() ) );
	}
}