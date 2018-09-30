//
// PhysicsPlaneComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Plane.h"

namespace dd
{
	struct PhysicsPlaneComponent
	{
		Plane Plane;
		float Elasticity { 1.0f };

		DD_CLASS( dd::PhysicsPlaneComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::PhysicsPlaneComponent, Plane );
			DD_MEMBER( dd::PhysicsPlaneComponent, Elasticity );
		}
	};
}
