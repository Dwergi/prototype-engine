//
// PhysicsSphereComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Sphere.h"

namespace dd
{
	struct PhysicsSphereComponent
	{
		glm::vec3 Velocity;
		glm::vec3 Acceleration;

		float Mass { 1.0 };
		float Elasticity { 1.0 };
		Sphere Sphere;

		bool Resting { false };

		DD_CLASS( dd::PhysicsSphereComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::PhysicsSphereComponent, Velocity );
			DD_MEMBER( dd::PhysicsSphereComponent, Acceleration );
			DD_MEMBER( dd::PhysicsSphereComponent, Mass );
			DD_MEMBER( dd::PhysicsSphereComponent, Elasticity );
			DD_MEMBER( dd::PhysicsSphereComponent, Sphere );
			DD_MEMBER( dd::PhysicsSphereComponent, Resting );
		}
	};
}