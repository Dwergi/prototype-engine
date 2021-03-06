//
// AABB2D.h
// Copyright (C) Sebastian Nordgren 
// August 11th 2018
//

#pragma once

namespace ddm
{
	struct Circle;

	struct AABB2D
	{
		AABB2D();
		AABB2D(glm::vec2 min, glm::vec2 max);
		AABB2D(const AABB2D& other);

		AABB2D GetTransformed(const glm::mat3& transform) const;

		glm::vec2 HalfExtents() const;

		bool Intersects(const ddm::AABB2D& other) const;
		bool Intersects(const ddm::Circle& circle) const;

		bool Contains(glm::vec2 pt) const;

		glm::vec2 NearestPoint(glm::vec2 pt) const;

		glm::vec2 Min { 0, 0 };
		glm::vec2 Max { 1, 1 };

		// Get the nearest normal to the direction.
		static glm::vec2 NearestNormal(glm::vec2 dir);

		DD_BEGIN_CLASS(ddm::AABB2D)
			DD_MEMBER(Min);
			DD_MEMBER(Max);
		DD_END_CLASS()
	};
}