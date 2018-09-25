//
// RayComponent.h - A ray component with an optional length.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "ComponentType.h"
#include "Ray.h"

namespace dd
{
	struct RayComponent
	{
		Ray Ray;

		//
		// How long the ray should be. 
		// Infinite if negative.
		//
		float Length; 

		DD_COMPONENT;
	};
}
