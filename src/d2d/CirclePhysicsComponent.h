//
// CirclePhysicsComponent.h - Component for the physics of a 2D circle.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#pragma once

#include "d2d/Physics2DBase.h"

#include "ddm/Circle.h"

namespace d2d
{
	struct CirclePhysicsComponent : d2d::Physics2DBase
	{
		// Hit circle is relative to the size of the sprite.
		ddm::Circle HitCircle;

		DD_BEGIN_CLASS(d2d::CirclePhysicsComponent)
			DD_COMPONENT();
			DD_PARENT(d2d::Physics2DBase);

			DD_MEMBER(HitCircle); 
		DD_END_CLASS()
	};
}