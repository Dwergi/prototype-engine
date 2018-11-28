#pragma once

namespace dd
{
	struct AABB;
	struct Sphere;
	struct BoundBoxComponent;
	struct BoundSphereComponent;
	struct TransformComponent;

	//
	// Retrieve the bounding box and bounding sphere from the given components.
	// If both are defined, returns those. If only one, then the other is filled in.
	// Returns true if bounds are valid, false if neither is defined.
	// 
	bool GetWorldBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere, 
		const dd::TransformComponent& transform, dd::AABB& aabb, dd::Sphere& sphere );

	bool GetBoundBoxAndSphere( const dd::BoundBoxComponent* bound_box, const dd::BoundSphereComponent* bound_sphere,
		dd::AABB& aabb, dd::Sphere& sphere );
}