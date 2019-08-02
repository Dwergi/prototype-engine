//
// PlaneComponent.h - A ray component with an optional length.
// Copyright (C) Sebastian Nordgren 
// March 10th 2019
//

#pragma once

#include "Plane.h"

namespace dd
{
	struct PlaneComponent
	{
		ddm::Plane Plane;

		DD_CLASS( dd::PlaneComponent )
			DD_COMPONENT();

			DD_MEMBER( Plane );
		DD_END_CLASS()
	};
}
