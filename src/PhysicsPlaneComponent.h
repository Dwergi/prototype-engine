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
		ddm::Plane Plane;
		float Elasticity { 1.0f };

		DD_BEGIN_CLASS( dd::PhysicsPlaneComponent )
			DD_COMPONENT();

			DD_MEMBER( Plane );
			DD_MEMBER( Elasticity );
		DD_END_CLASS()
	};
}
