//
// PhysicsPlaneComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "ComponentType.h"
#include "Plane.h"

namespace dd
{
	struct PhysicsPlaneComponent
	{
		Plane Plane;
		float Elasticity { 1.0f };

		DD_COMPONENT;
	};
}
