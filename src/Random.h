//
// Utility.h - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "pcg/pcg_random.hpp"

namespace dd
{
	class Random
	{
	public:

		static const uint DEFAULT_SEED = -1;

		Random( uint min, uint max, uint seed = DEFAULT_SEED );

		int Next();

	private:

		pcg64 m_rng;
	};
}