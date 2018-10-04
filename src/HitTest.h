//
// HitTest.cpp - Utilities for hit testing things.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

namespace ddr
{
	struct Mesh;
}

namespace dd
{
	struct AABB;
	struct BoundBoxComponent;
	struct BoundSphereComponent;
	struct MeshComponent;
	struct Ray;
	struct Sphere;
	struct TransformComponent;
}

namespace ddm
{
	bool HitTestMesh( const dd::Ray& ray, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
		const dd::BoundSphereComponent* bsphere_cmp, const dd::BoundBoxComponent* bbox_cmp,
		float& out_distance, glm::vec3& out_normal );

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform,
		const dd::Sphere& bound_sphere, const dd::AABB& bound_box,
		const ddr::Mesh& mesh, float& out_distance, glm::vec3& out_normal );
}