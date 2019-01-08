//
// RenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 13th 2018
//

#pragma once

#include "RenderState.h"

namespace ddr
{
	uint DistanceToDepth( float distance, bool transparent );

	enum class CommandType
	{
		Invalid,
		Mesh
	};

	constexpr int DEPTH_BITS = 24;

	struct CommandKey
	{
		union
		{
			struct
			{
				uint Layer : 2;
				uint Opaque : 1;
				uint Depth : DEPTH_BITS;
				uint Material : 24;
			};
			uint64 Key { 0 };
		};
	};

	struct RenderCommand
	{
		CommandKey Key;
		CommandType Type;
		RenderState RenderState;

		RenderCommand( CommandType type );
	};
}