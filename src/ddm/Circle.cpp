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
	Circle::Circle()
	{

	}

	Circle::Circle(glm::vec2 centre, float radius) :
		Centre(centre),
		Radius(radius)
	{

	}

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
		float distance = glm::distance(Centre, other.Centre);

		return distance < r;
	}
}