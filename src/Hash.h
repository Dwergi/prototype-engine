//
// Hash.h - An implementation of FNV1a hashing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	uint64 HashBytes( const byte* data, uint64 size );

	template <typename T>
	inline uint64 Hash( const T& value )
	{
		return HashBytes( (const byte*)((const void*) &value), sizeof( T ) );
	}

	template <>
	inline uint64 Hash( const uint64& value )
	{
		return value;
	}

	inline uint64 HashString( const char* data, uint64 size )
	{
		return dd::HashBytes( (const byte*) data, size );
	}
}