//
// TrenchSystem.h - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "ISystem.h"
#include "EntityHandle.h"
#include "MeshHandle.h"
#include "ShaderHandle.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

namespace ddr
{
	class ICamera;
}

namespace dd
{
	class Mesh;

	class TrenchSystem : public ISystem
	{
	public: 

		TrenchSystem();
		TrenchSystem( const TrenchSystem& other ) = delete;
		~TrenchSystem();

		void Update( EntityManager& entity_manager, float delta_t ) override;
		void CreateRenderResources();

	private:

		ddr::MeshHandle m_chunkMesh;
		ddr::ShaderHandle m_shader;

		glm::vec3 m_trenchDirection;
		glm::vec3 m_trenchOrigin;
		DenseMap<glm::vec3, EntityHandle> m_chunks;

		EntityHandle CreateTrenchChunk( glm::vec3 position, EntityManager& entity_manager );
	};
}