//
// TerrainChunkComponent.cpp
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#include "PrecompiledHeader.h"
#include "TerrainChunkComponent.h"

#include "TerrainChunk.h"

DD_COMPONENT_CPP( dd::TerrainChunkComponent );

namespace dd
{
	TerrainChunkComponent::TerrainChunkComponent()
	{

	}

	TerrainChunkComponent::TerrainChunkComponent( const TerrainChunkComponent& other )
	{
		Chunk = other.Chunk;
	}
}