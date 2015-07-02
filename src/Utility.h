#pragma once

#include <random>

template< typename TVector, typename TIterator >
void erase_unordered( TVector& container, TIterator it )
{
	auto item = container.back();
	std::swap( *it, item );

	container.pop_back();
}

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

bool operator==( const std::string& a, const std::string& b );