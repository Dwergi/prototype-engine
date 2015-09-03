//
// Hash.cpp - An implementation of FNV1a hashing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Hash.h"

uint64 dd::HashBytes( const uint8* data, uint64 size )
{
	const uint64_t fnv_prime = 1099511628211u;
	const uint64_t fnv_offset_basis = 14695981039346656037u;

	uint64_t hash = fnv_offset_basis;

	for( int i = 0; i < size; ++i )
	{
		hash ^= data[ i ];
		hash *= fnv_prime;
	}

	return hash;
}