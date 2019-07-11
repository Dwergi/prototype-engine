//
// PhysicsRigidBodyComponent.h
// Copyright (C) Sebastian Nordgren 
// October 8th 2018
//

#pragma once

#include "PhysicsBase.h"

namespace dd
{
	struct PhysicsRigidBodyComponent : PhysicsBase
	{
		glm::vec4 AngularMomentum;

		glm::quat Spin;

		float AngularInertia { 0.0f };

		// Offset relative to placement point.
		glm::vec3 CentreOfMass; 

		DD_BEGIN_CLASS( dd::PhysicsRigidBodyComponent )
			DD_PARENT( dd::PhysicsBase );
			DD_COMPONENT();

			DD_MEMBER( AngularMomentum );
			DD_MEMBER( Spin );
			DD_MEMBER( AngularInertia );
			DD_MEMBER( CentreOfMass );
		DD_END_CLASS()
	};
}