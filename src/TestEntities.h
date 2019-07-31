#pragma once

#include "Entity.h"
#include "Mesh.h"

namespace dd
{
	struct TestEntities
	{
		static ddc::Entity CreateMeshEntity(ddc::World& world, const ddr::MeshHandle& mesh_h, glm::vec4 colour, glm::vec3 pos, glm::quat rot, glm::vec3 scale);
		static ddc::Entity CreateBall(ddc::World& world, glm::vec3 translation, glm::vec4 colour, float size);
		static void CreatePhysicsPlaneTestScene(ddc::World& world);
		static void CreateAxes(ddc::World& world);
	};
}