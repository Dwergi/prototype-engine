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
			LOD( 0 ) {}

		float X;
		float Y;
		float Size;
		int LOD;

		bool operator==( const dd::TerrainChunkKey& other ) const;
	};

	template <>
	uint64 Hash( const TerrainChunkKey& key );
}

bool operator<( const dd::TerrainChunkKey& a, const dd::TerrainChunkKey& b );


namespace std
{
	template <>
	struct hash<dd::TerrainChunkKey>
	{
		std::size_t operator()( const dd::TerrainChunkKey& key ) const
		{
			return (std::size_t( key.X ) << 36) + (std::size_t( key.Y ) << 8) + ~(key.LOD & 0xff);
		}
	};
}
