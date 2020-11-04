//
// BoundSphereComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "ddm/Sphere.h"

namespace dd
{
	struct BoundSphereComponent
	{
		ddm::Sphere Sphere;

		DD_BEGIN_CLASS( dd::BoundSphereComponent )
			DD_COMPONENT();

			DD_MEMBER( Sphere );
		DD_END_CLASS()
	};

	DD_SERIALIZABLE( dd::BoundSphereComponent );
}