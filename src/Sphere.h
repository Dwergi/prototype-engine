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

	struct Sphere
	{
		Sphere();
		Sphere( const Sphere& other );
		Sphere( glm::vec3 centre, float radius );
		explicit Sphere( const AABB& aabb );

		bool IsValid() const { return Radius > 0; }

		bool Contains( const glm::vec3& pt ) const;

		bool Intersects( const Sphere& sphere ) const;

		bool IntersectsRay( const Ray& ray ) const;
		bool IntersectsRay( const Ray& ray, glm::vec3& out_position, glm::vec3& out_normal ) const;

		Sphere GetTransformed( const glm::mat4& t ) const;

		glm::vec3 Centre;
		float Radius;

		DD_CLASS( dd::Sphere )
		{
			DD_MEMBER( dd::Sphere, Centre );
			DD_MEMBER( dd::Sphere, Radius );
		}
	};
}