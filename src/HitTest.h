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
	struct BoundBoxComponent;
	struct BoundSphereComponent;
	struct MeshComponent;
	struct TransformComponent;
}

namespace ddm
{
	struct AABB;
	struct Ray;
	struct Sphere;

	bool HitTestMesh( const ddm::Ray& ray, const dd::MeshComponent& mesh_cmp, const dd::TransformComponent& transform_cmp,
		const dd::BoundSphereComponent* bsphere_cmp, const dd::BoundBoxComponent* bbox_cmp,
		float& out_distance, glm::vec3& out_normal );

	bool HitTestMesh( const ddm::Ray& ray, const glm::mat4& mesh_transform,
		const ddm::Sphere& bound_sphere, const ddm::AABB& bound_box,
		const ddr::Mesh& mesh, float& out_distance, glm::vec3& out_normal );
}