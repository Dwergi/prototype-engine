//
// Sphere.h - A bounding sphere.
// Copyright (C) Sebastian Nordgren 
// September 13th 2018
//

#pragma once

namespace ddm
{
	struct AABB;
	struct Ray;

	struct Sphere
	{
		Sphere();
		Sphere( const Sphere& other );
		Sphere( glm::vec3 centre, float radius );
		explicit Sphere( const ddm::AABB& aabb );

		bool IsValid() const { return Radius > 0; }

		bool Contains( const glm::vec3& pt ) const;

		bool Intersects( const Sphere& sphere ) const;
		bool Intersects( const ddm::AABB& aabb ) const;

		bool IntersectsRay( const ddm::Ray& ray ) const;
		bool IntersectsRay( const ddm::Ray& ray, glm::vec3& out_position, glm::vec3& out_normal ) const;

		Sphere GetTransformed( const glm::mat4& t ) const;

		glm::vec3 Centre;
		float Radius { 0.0f };

		DD_BEGIN_CLASS( ddm::Sphere )
			DD_MEMBER( Centre );
			DD_MEMBER( Radius );
		DD_END_CLASS()
	};
}