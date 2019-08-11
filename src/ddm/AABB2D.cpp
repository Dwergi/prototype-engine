//
// AABB2D.cpp
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#include "PCH.h"
#include "AABB2D.h"

DD_POD_CPP(ddm::AABB2D);

namespace ddm
{
	ddm::AABB2D AABB2D::GetTransformed(const glm::mat3& transform) const
	{
		AABB2D transformed;
		transformed.Min = (transform * glm::vec3(Min, 1)).xy; 
		transformed.Max = (transform * glm::vec3(Max, 1)).xy;
		return transformed;
	}

	bool AABB2D::Intersects(const AABB2D& other) const
	{
		return glm::all(glm::lessThanEqual(Min, other.Max)) && 
			glm::all(glm::greaterThanEqual(Max, other.Min));
	}

	glm::vec2 ddm::AABB2D::NearestPoint(glm::vec2 pt) const
	{
		return glm::clamp(pt, Min, Max);
	}

	glm::vec2 ddm::AABB2D::NearestNormal(glm::vec2 dir) const
	{
		if (std::abs(dir.x) > std::abs(dir.y))
		{
			// closer on x than y
			if (dir.x < 0) { return glm::vec2(-1, 0); }
			else { return glm::vec2(1, 0); }
		}
		else
		{
			if (dir.y < 0) { return glm::vec2(0, -1); }
			else { return glm::vec2(0, 1); }
		}
	}
}