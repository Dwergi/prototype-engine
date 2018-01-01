//
// Math_dd.h - Min and max, for reasons.
// Copyright (C) Sebastian Nordgren 
// October 25th 2017
//

#pragma once

namespace dd
{
#undef min
#undef max

	template <typename T>
	T min( T a, T b )
	{
		return a <= b ? a : b;
	}

	template <typename T>
	T max( T a, T b )
	{
		return a >= b ? a : b;
	}

	template <typename T>
	T wrap( T value, T min, T max )
	{
		value = min + std::fmod( value - min, max - min );

		if( value < 0 )
			value += max;

		return value;
	}
}