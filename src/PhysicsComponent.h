//
// PhysicsComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Sphere.h"

namespace dd
{
	struct PhysicsComponent
	{
		glm::vec3 Velocity;
		glm::vec3 Acceleration;

		float Mass { 1.0 };
		float Elasticity { 1.0 };
		Sphere Sphere;

		DD_COMPONENT;
	};
}