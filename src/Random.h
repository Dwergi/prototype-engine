//
// Utility.h - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "pcg/pcg_random.hpp"

namespace dd
{
	constexpr uint DEFAULT_SEED_32 = ~0;
	constexpr uint DEFAULT_SEED_64 = ~0;

	struct Random64
	{
		Random64( uint64 seed = DEFAULT_SEED_64 );
		Random64( uint64 min, uint64 max, uint64 seed = DEFAULT_SEED_64 );

		uint64 Next();

		uint64 Max() const { return m_max; }
		void SetMax( uint64 max ) { m_max = max; }

		uint64 Min() const { return m_min; }
		void SetMin( uint64 min ) { m_min = min; }

	private:

		pcg64_fast m_rng;

		uint64 m_min;
		uint64 m_max;
	};

	template <typename T>
	struct Random32Base
	{
		Random32Base( uint seed = DEFAULT_SEED_32 );
		Random32Base( T min, T max, uint seed = DEFAULT_SEED_32 );

		virtual T Next() = 0;

		T Min() const { return m_min; }
		void SetMin( T min ) { m_min = min; }

		T Max() const { return m_max; }
		void SetMax( T max ) { m_max = max; }

	protected:
		pcg32_fast m_rng;

		T m_min;
		T m_max;
	};

	struct Random32 : Random32Base<uint>
	{
		Random32( uint seed = DEFAULT_SEED_32 );
		Random32( uint min, uint max, uint seed = DEFAULT_SEED_32 );

		virtual uint Next() override;
	};

	struct RandomInt : Random32Base<int>
	{
		RandomInt( uint seed = DEFAULT_SEED_32 );
		RandomInt( int min, int max, uint seed = DEFAULT_SEED_32 );

		virtual int Next() override;
	};

	struct RandomFloat : Random32Base<float>
	{
		RandomFloat( uint seed = DEFAULT_SEED_32 );
		RandomFloat( float min, float max, uint seed = DEFAULT_SEED_32 );

		virtual float Next() override;
	};

	struct RandomVector3
	{
		RandomVector3( uint seed = DEFAULT_SEED_32 );
		RandomVector3( glm::vec3 min, glm::vec3 max, uint seed = DEFAULT_SEED_32 );

		glm::vec3 Next();

		glm::vec3 Min() const;
		void SetMin( glm::vec3 min );

		glm::vec3 Max() const;
		void SetMax( glm::vec3 max );

	private:
		RandomFloat m_rngX;
		RandomFloat m_rngY;
		RandomFloat m_rngZ;
	};

	glm::vec2 GetRandomVector2( dd::RandomFloat& rng, glm::vec2 min, glm::vec2 max );
	glm::vec3 GetRandomVector3( dd::RandomFloat& rng, glm::vec3 min, glm::vec3 max );

	// 
	// INLINE IMPLEMENTATION
	//

	template <typename T>
	Random32Base<T>::Random32Base( uint seed ) :
		m_rng( seed ),
		m_min( std::numeric_limits<T>::min() ),
		m_max( std::numeric_limits<T>::max() )
	{
		if( seed == DEFAULT_SEED_32 )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}

	template <typename T>
	Random32Base<T>::Random32Base( T min, T max, uint seed ) :
		m_rng( seed ),
		m_min( min ),
		m_max( max )
	{
		DD_ASSERT( m_min < m_max );

		if( seed == DEFAULT_SEED_32 )
		{
			m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
		}
	}
}