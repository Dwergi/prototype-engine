//
// Hash.h - An implementation of FNV1a hashing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <string>

typedef unsigned long long uint64;
typedef unsigned char byte;

namespace dd
{
	uint64 HashBytes( const byte* data, uint64 size );

	template< typename T >
	uint64 Hash( const T& value )
	{
		return HashBytes( (const byte*)((const void*) &value), sizeof( T ) );
	}

	template<>
	inline uint64 Hash( const std::string& value )
	{
		HashBytes( (const byte*) value.c_str(), value.size() );
	}
}