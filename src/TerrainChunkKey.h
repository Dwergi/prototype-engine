//
// TerrainChunkKey.h - A key to a chunk of terrain.
// Copyright (C) Sebastian Nordgren 
// April 21st 2016
//

#pragma once

namespace dd
{
	struct TerrainChunkKey
	{
		TerrainChunkKey() :
			X( 0xFFFFFFF ),
			Y( 0xFFFFFFF ),
			Size( 0 ),
			IsSplit( false ) {}

		int64 X;
		int64 Y;
		int Size;
		bool IsSplit;
	};

	template <>
	uint64 Hash( const TerrainChunkKey& key );
}

bool operator<( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b );
bool operator==( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b );