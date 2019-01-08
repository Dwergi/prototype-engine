//
// RenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 13th 2018
//

#pragma once

#include "Material.h"

namespace ddr
{
	uint DistanceToDepth( float distance, bool transparent );

	enum class CommandType
	{
		Invalid,
		Mesh
	};

	constexpr int DEPTH_BITS = 24;
	constexpr int MATERIAL_BITS = 8;

	struct CommandKey
	{
		union
		{
			struct
			{
				uint Layer : 2;
				uint Opaque : 1;
				uint Depth : DEPTH_BITS;
				uint Material : MATERIAL_BITS;
			};
			uint64 Key { 0 };
		};
	};

	struct RenderCommand
	{
		CommandKey Key;
		CommandType Type;
		MaterialHandle Material;

		RenderCommand( CommandType type );
	};
}