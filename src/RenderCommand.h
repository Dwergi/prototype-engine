//
// RenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 13th 2018
//

#pragma once

namespace ddr
{
	enum class CommandType
	{
		Invalid,
		Mesh
	};

	struct CommandKey
	{
		union
		{
			struct
			{
				uint Layer : 2;
				uint Opaque : 1;
				uint Depth : 24;
				uint Material : 24;
			};
			uint64 Key { 0 };
		};
	};

	struct RenderCommand
	{
		CommandKey Key;
		CommandType Type;
	};
}