//
// TerrainChunkComponent.h
// Copyright (C) Sebastian Nordgren 
// January 6th 2018
//

#pragma once

namespace neut
{
	struct TerrainChunk;

	struct TerrainChunkComponent
	{
		TerrainChunk* Chunk { nullptr };

		DD_BEGIN_CLASS(neut::TerrainChunkComponent )
			DD_COMPONENT();
		DD_END_CLASS()
	};
}