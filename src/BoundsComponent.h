#pragma once

#include "AABB.h"
#include "ComponentType.h"

namespace dd
{
	struct BoundsComponent
	{
		DD_COMPONENT;

		AABB Local;
		AABB World;

		DD_BEGIN_TYPE( BoundsComponent )
			DD_MEMBER( BoundsComponent, Local )
			DD_MEMBER( BoundsComponent, World )
		DD_END_TYPE
	};
}