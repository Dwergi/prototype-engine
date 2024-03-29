//
// BoundsComponent.h - A component that contains a bunch of bounds for an entity.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

#include "ddm/AABB.h"

namespace dd
{
	struct BoundBoxComponent
	{
		ddm::AABB BoundBox;

		DD_BEGIN_CLASS( dd::BoundBoxComponent )
			DD_COMPONENT();

			DD_MEMBER( BoundBox );
		DD_END_CLASS()
	};
}