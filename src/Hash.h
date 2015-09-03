//
// Hash.h - An implementation of FNV1a hashing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <string>

typedef unsigned long long uint64;

namespace dd
{
	uint64 HashBytes( const uint8* data, uint64 size );

	template< typename T >
	inline uint64 Hash( const T& value )
	{
		return HashBytes( (const byte*)((const void*) &value), sizeof( T ) );
	}

	template<>
	inline uint64 Hash( const dd::String& value )
	{
		return HashBytes( (const uint8*) value.c_str(), value.Length() );
	}
}