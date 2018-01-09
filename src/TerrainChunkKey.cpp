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
		// This should mean that in most cases chunks are ordered by X, then Y, then inverse LOD
		return (int64(key.X) << 36) + (int64(key.Y) << 8) + ~(key.LOD & 0xff);
	}

	bool TerrainChunkKey::operator==( const dd::TerrainChunkKey& other ) const
	{
		if( dd::Hash( *this ) == dd::Hash( other ) )
		{
			return LOD == other.LOD &&
				X == other.X &&
				Y == other.Y;
		}

		return false;
	}
}

bool operator<( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b )
{
	return dd::Hash( a ) < dd::Hash( b );
}
