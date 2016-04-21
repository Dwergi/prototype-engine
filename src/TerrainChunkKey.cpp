//
// TerrainChunkKey.cpp - A key to a chunk of terrain.
// Copyright (C) Sebastian Nordgren 
// April 21st 2016
//

#include "PrecompiledHeader.h"
#include "TerrainChunkKey.h"

namespace dd
{
	template <>
	uint64 Hash( const TerrainChunkKey& key )
	{
		// Hash the bottom 28 bytes of the X and Y coordinates, and the inverse of the bottom 8 bytes of the size
		// This should mean that in most cases chunks are ordered by X, then Y, then inverse size
		return (key.X << 36) + (key.Y << 8) + ~(key.Size & 0xff);
	}
}

bool operator<( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b )
{
	return dd::Hash( a ) < dd::Hash( b );
}

bool operator==( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b )
{
	if( dd::Hash( a ) == dd::Hash( b ) )
	{
		return a.IsSplit == b.IsSplit &&
			a.Size == b.Size &&
			a.X == b.X &&
			a.Y == b.Y;
	}

	return false;
}