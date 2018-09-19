//
// BoundSphere.cpp - A bounding sphere.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#include "PrecompiledHeader.h"
#include "Sphere.h"

#include "AABB.h"
#include "Ray.h"

#include <glm/gtx/intersect.hpp>

namespace dd
{
	Sphere::Sphere()
	{

	}

	Sphere::Sphere( const Sphere& other ) :
		Centre( other.Centre ),
		Radius( other.Radius )
	{
	}

	Sphere::Sphere( glm::vec3 centre, float radius )
	{
		Centre = centre;
		Radius = radius;
	}

	Sphere::Sphere( const AABB& aabb )
	{
		Centre = aabb.Center();
		Radius = glm::length( aabb.Extents() ) / 2.0f;
	}

	bool Sphere::Contains( const glm::vec3& pt ) const
	{
		return glm::distance2( pt, Centre ) < (Radius * Radius);
	}

	bool Sphere::Intersects( const Sphere& sphere ) const
	{
		return (glm::length( sphere.Centre - Centre ) + sphere.Radius) <= Radius;
	}

	bool Sphere::IntersectsRay( const Ray& ray ) const
	{
		glm::vec3 pos, normal;
		return IntersectsRay( ray, pos, normal );
	}

	bool Sphere::IntersectsRay( const Ray& ray, glm::vec3& out_position, glm::vec3& out_normal ) const
	{
		return glm::intersectRaySphere( ray.Origin(), ray.Direction(), Centre, Radius, out_position, out_normal );
	}

	Sphere Sphere::GetTransformed( const glm::mat4& t ) const
	{
		glm::vec4 centre = t * glm::vec4( Centre, 1 );
		glm::vec4 radius = t * glm::vec4( Radius, Radius, Radius, 0 );

		return Sphere( glm::vec3( centre.xyz ), dd::max( radius.x, dd::max( radius.y, radius.z ) ) );
	}
}