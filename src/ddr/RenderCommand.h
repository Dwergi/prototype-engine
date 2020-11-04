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
	constexpr int MESH_BITS = 12;
	constexpr int MATERIAL_BITS = 12;

	struct CommandKey
	{
		union
		{
			struct
			{
				uint64 Layer : 2;
				uint64 Opaque : 1;
				uint64 Depth : DEPTH_BITS;
				uint64 Mesh : MESH_BITS;
				uint64 Material : MATERIAL_BITS;
			};
			uint64 Key { 0 };
		};

		bool operator>(const CommandKey& other) const
		{
			return Key > other.Key;
		}

		bool operator<(const CommandKey& other) const
		{
			return Key < other.Key;
		}
	};

	struct RenderCommand
	{
		CommandKey Key;
		CommandType Type;
		MaterialHandle Material;

		RenderCommand( CommandType type );
	};
}