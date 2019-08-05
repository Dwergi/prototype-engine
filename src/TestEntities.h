#pragma once

#include "Mesh.h"

namespace dd
{
	struct TestEntities
	{
		static ddc::Entity CreateMeshEntity(ddc::EntitySpace& entities, const ddr::MeshHandle& mesh_h, glm::vec4 colour, glm::vec3 pos, glm::quat rot, glm::vec3 scale);
		static ddc::Entity CreateBall(ddc::EntitySpace& entities, glm::vec3 translation, glm::vec4 colour, float size);
		static void CreatePhysicsPlaneTestScene(ddc::EntitySpace& entities);
		static void CreateAxes(ddc::EntitySpace& entities);
	};
}