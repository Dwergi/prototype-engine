#pragma once

namespace ddr
{
	struct Mesh;
}

namespace dd
{
	struct AABB;
	struct Sphere; 
	struct Ray;

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform,
		const dd::Sphere& bound_sphere, const dd::AABB& bound_box,
		const ddr::Mesh& mesh, float& out_distance );
}