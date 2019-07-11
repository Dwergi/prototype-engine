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
		ddm::Sphere Sphere;

		DD_BEGIN_CLASS( dd::PhysicsSphereComponent )
			DD_PARENT( dd::PhysicsBase );
			DD_COMPONENT();

			DD_MEMBER( Sphere );
		DD_END_CLASS()
	};
}