//
// TerrainChunkComponent.h
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#pragma once

#include "TerrainChunkKey.h"

namespace dd
{
	class TerrainChunk;

	struct TerrainChunkComponent
	{
		TerrainChunk* Chunk { nullptr };

		DD_CLASS( dd::TerrainChunkComponent )
		{
			DD_COMPONENT();
		}
	};
}