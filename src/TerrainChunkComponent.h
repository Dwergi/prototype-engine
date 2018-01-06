#pragma once

#include "IComponent.h"
#include "DenseMapPool.h"
#include "TerrainChunkKey.h"

namespace dd
{
	class TerrainChunkComponent : public IComponent
	{
	public:

		using Pool = DenseMapPool<TerrainChunkComponent>;

		TerrainChunkComponent();

		TerrainChunkKey Key;
		bool IsActive { false };

		BASIC_TYPE( TerrainChunkComponent )
	};
}