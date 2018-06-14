//
// TerrainChunkComponent.h
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#pragma once

#include "IComponent.h"
#include "PackedPool.h"
#include "TerrainChunkKey.h"

namespace dd
{
	class TerrainChunk;

	class TerrainChunkComponent : public IComponent
	{
	public:

		using Pool = PackedPool<TerrainChunkComponent>;

		TerrainChunkComponent();

		TerrainChunk* Chunk { nullptr };
		bool IsActive { false };

		BASIC_TYPE( TerrainChunkComponent )
	};
}