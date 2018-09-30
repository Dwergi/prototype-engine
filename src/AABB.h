//
// AABB.h - An axis-aligned bounding box.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	struct Sphere;
	struct Ray;

	struct AABB
	{
		AABB();
		AABB( glm::vec3 min, glm::vec3 max );
		AABB( const AABB& other );
		explicit AABB( const dd::Sphere& sphere );

		void Clear();

		//
		// Is this a non-zero volume bounding box.
		//
		bool IsValid() const;

		//
		// Expand the bounding box to contain the given point.
		//
		void Expand( const glm::vec3& pt );

		//
		// Expand the bounding box to contain the given bounding box.
		//
		void Expand( const AABB& bounds );

		bool Contains( const glm::vec3& pt ) const;
		bool Contains( const AABB& other ) const;

		bool Intersects( const Sphere& sphere ) const;
		bool Intersects( const AABB& bb ) const;
		bool IntersectsRay( const glm::vec3& start, const glm::vec3& dir, float& distance ) const;
		bool IntersectsRay( const Ray& ray ) const;
		bool IntersectsRay( const Ray& ray, float& distance ) const;

		glm::vec3 Center() const { return (Min + Max) * 0.5f; }
		glm::vec3 Extents() const { return Max - Min; }
		float Volume() const;

		AABB GetTransformed( const glm::mat4& transform ) const;
		void GetCorners( glm::vec3 (&corners)[8] ) const;

		bool operator==( const AABB& other ) const;
		bool operator!=( const AABB& other ) const;

		glm::vec3 Min;
		glm::vec3 Max;

		DD_CLASS( dd::AABB )
		{
			DD_MEMBER( dd::AABB, Min );
			DD_MEMBER( dd::AABB, Max );
		}
	};
}