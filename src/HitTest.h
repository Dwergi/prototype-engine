#pragma once

namespace ddr
{
	class Mesh;
}

namespace dd
{
	struct AABB;
	struct Ray;

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform, const dd::AABB& bounds, const ddr::Mesh& mesh, float& out_distance );
}