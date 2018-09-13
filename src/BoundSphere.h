//
// BoundSphere.h - A bounding sphere.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

namespace dd
{
	struct AABB;
	struct Ray;

	struct BoundSphere
	{
		BoundSphere();
		BoundSphere( const BoundSphere& other );
		BoundSphere( glm::vec3 centre, float radius );
		explicit BoundSphere( const AABB& aabb );

		bool IsValid() const { return m_radius != 0; }

		bool Contains( const glm::vec3& pt ) const;

		bool Intersects( const BoundSphere& sphere ) const;

		bool IntersectsRay( const Ray& ray ) const;
		bool IntersectsRay( const Ray& ray, glm::vec3& out_position, glm::vec3& out_normal ) const;

		glm::vec3 Centre() const { return m_centre; }
		float Radius() const { return m_radius; }

	private:
		glm::vec3 m_centre;
		float m_radius { 0 };
	};
}