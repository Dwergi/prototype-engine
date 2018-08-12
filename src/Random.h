//
// Utility.h - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "pcg/pcg_random.hpp"

namespace dd
{
	class Random64
	{
	public:

		static const uint64 DEFAULT_SEED = -1;

		Random64( uint64 seed = DEFAULT_SEED );
		Random64( uint64 min, uint64 max, uint64 seed = DEFAULT_SEED );

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

	class Random32
	{
	public:

		static const uint DEFAULT_SEED = ~0;

		Random32( uint seed = DEFAULT_SEED );
		Random32( uint min, uint max, uint seed = DEFAULT_SEED );

		uint Next();

		uint Max() const { return m_max; }
		void SetMax( uint max ) { m_max = max; }

		uint Min() const { return m_min; }
		void SetMin( uint min ) { m_min = min; }

	private:

		pcg32_fast m_rng;

		uint m_min;
		uint m_max;
	};

	class RandomInt
	{
	public:

		static const int DEFAULT_SEED = -1;

		RandomInt( int seed = DEFAULT_SEED );
		RandomInt( int min, int max, int seed = DEFAULT_SEED );

		int Next();

		int Max() const { return m_max; }
		void SetMax( int max ) { m_max = max; }

		int Min() const { return m_min; }
		void SetMin( int min ) { m_min = min; }

	private:

		pcg32_fast m_rng;

		int m_min;
		int m_max;
	};

	class RandomFloat
	{
	public:

		static const uint DEFAULT_SEED = -1;

		RandomFloat( uint seed = DEFAULT_SEED );
		RandomFloat( float min, float max, uint seed = DEFAULT_SEED );

		float Next();

		float Max() const { return m_max; }
		void SetMax( float max ) { m_max = max; }

		float Min() const { return m_min; }
		void SetMin( float min ) { m_min = min; }

	private:

		pcg32_fast m_rng;

		float m_min;
		float m_max;
	};
}