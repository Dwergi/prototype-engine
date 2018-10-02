//
// BoundSphereComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Sphere.h"

namespace dd
{
	struct BoundSphereComponent
	{
		Sphere Sphere;

		DD_CLASS( dd::BoundSphereComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::BoundSphereComponent, Sphere );
		}
	};
}