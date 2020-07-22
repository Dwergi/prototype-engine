//
// AABB2D.cpp
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#include "PCH.h"
#include "AABB2D.h"

#include "Circle.h"

DD_POD_CPP(ddm::AABB2D);

namespace ddm
{
	AABB2D::AABB2D()
	{

	}

	AABB2D::AABB2D(glm::vec2 min, glm::vec2 max) :
		Min(min),
		Max(max)
	{

	}

	AABB2D::AABB2D(const AABB2D& other) :
		Min(other.Min),
		Max(other.Max)
	{

	}

	AABB2D AABB2D::GetTransformed(const glm::mat3& transform) const
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

	bool AABB2D::Intersects(const Circle& other) const
	{
		if (Contains(other.Centre))
		{
			return true;
		}
		
		glm::vec2 nearest = NearestPoint(other.Centre);

		return glm::distance2(nearest, other.Centre) < other.Radius;
	}

	bool AABB2D::Contains(glm::vec2 pt) const
	{
		return glm::all(glm::lessThanEqual(Min, pt)) &&
			glm::all(glm::greaterThanEqual(Max, pt));
	}

	glm::vec2 AABB2D::NearestPoint(glm::vec2 pt) const
	{
		return glm::clamp(pt, Min, Max);
	}

	glm::vec2 AABB2D::HalfExtents() const
	{
		return (Max - Min) / 2.0f;
	}

	glm::vec2 AABB2D::NearestNormal(glm::vec2 dir)
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