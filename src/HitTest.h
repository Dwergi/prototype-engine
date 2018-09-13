#pragma once

namespace ddr
{
	class Mesh;
}

namespace dd
{
	struct AABB;
	struct BoundSphere; 
	struct Ray;

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform,
		const dd::BoundSphere& bound_sphere, const dd::AABB& bound_box,
		const ddr::Mesh& mesh, float& out_distance );
}