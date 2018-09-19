//
// BoundSphereComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Sphere.h"
#include "ComponentType.h"

namespace dd
{
	struct BoundSphereComponent
	{
		DD_COMPONENT;

		Sphere BoundSphere;
	};
}