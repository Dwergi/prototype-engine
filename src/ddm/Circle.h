//
// Circle.h
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#pragma once

namespace ddm
{
	struct Circle
	{
		Circle();
		Circle(glm::vec2 centre, float radius);

		Circle GetTransformed(glm::vec2 translate, glm::vec2 scale) const;

		bool Intersects(const Circle& other) const;

		glm::vec2 Centre { 0, 0 };
		float Radius { 0 };

		DD_BEGIN_CLASS(ddm::Circle)
			DD_MEMBER(Centre);
			DD_MEMBER(Radius);
		DD_END_CLASS()
	};
}