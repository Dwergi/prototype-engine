//
// AABB.h - An axis-aligned bounding box.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	struct AABB
	{
		AABB();
		AABB( glm::vec3 min, glm::vec3 max );
		AABB( const AABB& other );
		~AABB();

		//
		// Expand the bounding box to contain the given point.
		//
		void Expand( const glm::vec3& pt );
		void Expand( const AABB& bounds );

		bool Contains( const glm::vec3& pt ) const;
		bool Contains( const AABB& other ) const;

		bool Intersects( const AABB& other ) const;
		bool IntersectsRay( const glm::vec3& start, const glm::vec3& dir, float& distance ) const;

		glm::vec3 Center() const { return (Min + Max) * 0.5f; }
		glm::vec3 Extents() const { return Max - Min; }
		float Volume() const;

		AABB GetTransformed( const glm::mat4& transform ) const;
		void GetCorners( glm::vec3 (&corners)[8] ) const;

		bool operator==( const AABB& other ) const;
		bool operator!=( const AABB& other ) const;

		glm::vec3 Min;
		glm::vec3 Max;
	};
}