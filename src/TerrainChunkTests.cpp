//
// TerrainChunkTests.cpp - Tests for TerrainChunk.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "TerrainChunk.h"

TEST_CASE( "[TerrainChunk] Generate" )
{
	dd::ChunkKey key;
	key.X = 0;
	key.Y = 0;
	key.Size = 16;

	dd::TerrainChunk chunk( key );
	chunk.Generate();

	//chunk.Write( "terrain_chunk.tga" );
}