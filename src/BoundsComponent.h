//
// BoundsComponent.h - A component that contains a bunch of bounds for an entity.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

#include "AABB.h"
#include "BoundSphere.h"
#include "ComponentType.h"

namespace dd
{
	struct BoundsComponent
	{
		DD_COMPONENT;

		AABB LocalBox;
		AABB WorldBox;

		BoundSphere LocalSphere;
		BoundSphere WorldSphere;

		DD_BEGIN_TYPE( BoundsComponent )
			DD_MEMBER( BoundsComponent, LocalBox )
			DD_MEMBER( BoundsComponent, WorldBox )
			//DD_MEMBER( BoundsComponent, LocalSphere )
			//DD_MEMBER( BoundsComponent, WorldSphere )
		DD_END_TYPE
	};
}