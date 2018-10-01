//
// BoundSphere.cpp - A bounding sphere.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#include "PCH.h"
#include "Sphere.h"

#include "AABB.h"
#include "Ray.h"

#include <glm/gtx/intersect.hpp>

DD_TYPE_CPP( dd::Sphere );

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

	bool Sphere::Intersects( const AABB& aabb ) const
	{
		glm::vec3 closest = glm::clamp( Centre, aabb.Min, aabb.Max );

		return glm::distance2( closest, Centre ) < Radius * Radius;
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
		glm::vec4 x = t * glm::vec4( Radius, 0, 0, 0 );
		glm::vec4 y = t * glm::vec4( 0, Radius, 0, 0 );
		glm::vec4 z = t * glm::vec4( 0, 0, Radius, 0 );

		return Sphere( glm::vec3( centre.xyz ), std::sqrt( dd::max( glm::length2( x ), glm::length2( y ), glm::length2( z ) ) ) );
	}
}