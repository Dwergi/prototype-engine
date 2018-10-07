//
// PhysicsSphereComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "PhysicsBase.h"
#include "Sphere.h"

namespace dd
{
	struct PhysicsSphereComponent : PhysicsBase
	{
		Sphere Sphere;

		DD_CLASS( dd::PhysicsSphereComponent )
		{
			DD_PARENT( dd::PhysicsBase );
			DD_COMPONENT();

			DD_MEMBER( dd::PhysicsSphereComponent, Sphere );
		}
	};
}