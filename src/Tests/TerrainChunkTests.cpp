//
// TerrainChunkTests.cpp - Tests for TerrainChunk.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "catch2/catch.hpp"

#include "TerrainChunk.h"
#include "TerrainChunkKey.h"
#include "TerrainParameters.h"

TEST_CASE( "[TerrainChunk] Generate" )
{
	dd::TerrainChunkKey key;
	key.X = 0;
	key.Y = 0;
	key.LOD = 0;

	dd::TerrainParameters params;

	dd::TerrainChunk chunk( params, key );
	chunk.Generate();

	//chunk.Write( "terrain_chunk.tga" );
}