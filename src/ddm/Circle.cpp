//
// Circle.cpp
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#include "PCH.h"
#include "Circle.h"

DD_POD_CPP(ddm::Circle);

namespace ddm
{
	Circle Circle::GetTransformed(glm::vec2 translate, glm::vec2 scale) const
	{
		Circle result;
		result.Centre = Centre + translate;
		result.Radius = Radius * ddm::max(scale.x, scale.y);
		return result;
	}

	bool Circle::Intersects(const Circle& other) const
	{
		float r = Radius + other.Radius;
		r *= r;

		return r < glm::distance2(Centre, other.Centre);
	}
}