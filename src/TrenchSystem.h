//
// TrenchSystem.h - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "ISystem.h"
#include "EntityHandle.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

namespace dd
{
	class Camera;
	class Mesh;

	class TrenchSystem : public ISystem
	{
	public: 

		TrenchSystem( Camera& camera );
		~TrenchSystem();

		void Update( EntityManager& entity_manager, float delta_t ) override;
		void CreateRenderResources();

	private:

		MeshHandle m_chunkMesh;
		ShaderHandle m_shader;

		Camera& m_camera;
		glm::vec3 m_trenchDirection;
		glm::vec3 m_trenchOrigin;
		dd::DenseMap<glm::vec3, EntityHandle> m_chunks;

		EntityHandle CreateTrenchChunk( glm::vec3 position, EntityManager& entity_manager );
	};
}