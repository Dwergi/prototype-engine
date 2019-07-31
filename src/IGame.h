#pragma once

#include "World.h"

namespace ddc
{
	struct World;
}

namespace dd
{
	struct IGame
	{
		virtual void Initialize(ddc::World& world) = 0;
		virtual void Shutdown(ddc::World& world) = 0;
		virtual void Update(ddc::World& world) = 0;
		virtual void RenderUpdate(ddc::World& world) = 0;

		virtual const char* GetTitle() const { return "DD"; }
	};
}