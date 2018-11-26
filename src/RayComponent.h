//
// RayComponent.h - A ray component with an optional length.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "Ray.h"

namespace dd
{
	struct RayComponent
	{
		Ray Ray;

		DD_CLASS( dd::RayComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::RayComponent, Ray );
		}
	};
}
