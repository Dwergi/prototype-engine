//
// BoxPhysicsComponent.h - Component for the physics of a 2D box.
// Copyright (C) Sebastian Nordgren 
// August 4th 2019
//

#pragma once

#include "d2d/Physics2DBase.h"

#include "ddm/AABB2D.h"

namespace d2d
{
	struct BoxPhysicsComponent : d2d::Physics2DBase
	{
		ddm::AABB2D HitBox;

		DD_BEGIN_CLASS(d2d::BoxPhysicsComponent)
			DD_COMPONENT();
			DD_PARENT(d2d::Physics2DBase);

			DD_MEMBER(HitBox);
		DD_END_CLASS()
	};
}