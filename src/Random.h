//
// Utility.h - A random number generator.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <random>

namespace dd
{
	class Random
	{
		std::mt19937 engine;
		std::uniform_int_distribution<unsigned int> distribution;

	public:

		Random( unsigned int min, unsigned int max ) 
			: distribution( min, max )
		{

		}

		int Next()
		{
			return distribution( engine );
		}
	};
}