#pragma once

namespace ddr
{
	class Mesh;
}

namespace dd
{
	struct Ray;

	bool HitTestMesh( const dd::Ray& ray, const glm::mat4& mesh_transform, const ddr::Mesh& mesh, float& out_distance );
}