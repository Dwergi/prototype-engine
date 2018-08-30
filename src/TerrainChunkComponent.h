//
// TerrainChunkComponent.h
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#pragma once

#include "ComponentType.h"
#include "PackedPool.h"
#include "TerrainChunkKey.h"

namespace dd
{
	class TerrainChunk;

	class TerrainChunkComponent
	{
	public:

		TerrainChunkComponent();
		TerrainChunkComponent( const TerrainChunkComponent& other );

		TerrainChunk* Chunk { nullptr };

		DD_COMPONENT;

		BASIC_TYPE( TerrainChunkComponent )
	};
}