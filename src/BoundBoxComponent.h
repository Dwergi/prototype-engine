//
// BoundsComponent.h - A component that contains a bunch of bounds for an entity.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

#include "AABB.h"
#include "ComponentType.h"

namespace dd
{
	struct BoundBoxComponent
	{
		DD_COMPONENT;

		AABB BoundBox;

		DD_BEGIN_TYPE( BoundBoxComponent )
			DD_MEMBER( BoundBoxComponent, BoundBox )
		DD_END_TYPE
	};
}