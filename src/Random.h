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

		Random64( uint64 min, uint64 max, uint64 seed = DEFAULT_SEED );

		uint64 Next();

	private:

		pcg64_fast m_rng;

		uint64 m_min;
		uint64 m_max;
	};

	class Random32
	{
	public:

		static const uint DEFAULT_SEED = -1;

		Random32( uint min, uint max, uint seed = DEFAULT_SEED );

		uint Next();

	private:

		pcg32_fast m_rng;

		uint m_min;
		uint m_max;
	};

	class RandomInt
	{
	public:

		static const int DEFAULT_SEED = -1;

		RandomInt( int min, int max, int seed = DEFAULT_SEED );

		int Next();

	private:

		pcg32_fast m_rng;

		int m_min;
		int m_max;
	};
}