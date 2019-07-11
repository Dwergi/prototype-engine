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
		ddm::Ray Ray;

		DD_BEGIN_CLASS( dd::RayComponent )
			DD_COMPONENT();

			DD_MEMBER( Ray );
		DD_END_CLASS()
	};
}
