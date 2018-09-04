#pragma once

#include "AABB.h"
#include "ComponentType.h"

namespace dd
{
	struct BoundsComponent
	{
		DD_COMPONENT;

		AABB Bounds;

		DD_BEGIN_TYPE( BoundsComponent )
			DD_MEMBER( BoundsComponent, Bounds )
		DD_END_TYPE
	};
}