//
// BoundSphere.cpp - A bounding sphere.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#include "PrecompiledHeader.h"
#include "BoundSphere.h"

#include "AABB.h"
#include "Ray.h"

#include <glm/gtx/intersect.hpp>

namespace dd
{
	BoundSphere::BoundSphere()
	{

	}

	BoundSphere::BoundSphere( const BoundSphere& other ) :
		m_centre( other.m_centre ),
		m_radius( other.m_radius )
	{
	}

	BoundSphere::BoundSphere( glm::vec3 centre, float radius )
	{
		m_centre = centre;
		m_radius = radius;
	}

	BoundSphere::BoundSphere( const AABB& aabb )
	{
		m_centre = aabb.Center();
		m_radius = glm::length( aabb.Extents() ) / 2.0f;
	}

	bool BoundSphere::Contains( const glm::vec3& pt ) const
	{
		return glm::distance2( pt, m_centre ) < (m_radius * m_radius);
	}

	bool BoundSphere::Intersects( const BoundSphere& sphere ) const
	{
		return (glm::length( sphere.m_centre - m_centre ) + sphere.m_radius) <= m_radius;
	}

	bool BoundSphere::IntersectsRay( const Ray& ray ) const
	{
		glm::vec3 pos, normal;
		return IntersectsRay( ray, pos, normal );
	}

	bool BoundSphere::IntersectsRay( const Ray& ray, glm::vec3& out_position, glm::vec3& out_normal ) const
	{
		return glm::intersectRaySphere( ray.Origin(), ray.Direction(), m_centre, m_radius, out_position, out_normal );
	}
}