//
// TrenchSystem.h - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "EntityHandle.h"
#include "MeshHandle.h"
#include "ShaderHandle.h"
#include "System.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

namespace ddr
{
	class ICamera;
}

namespace dd
{
	class Mesh;

	class TrenchSystem : public ddc::System
	{
	public: 

		TrenchSystem();
		TrenchSystem( const TrenchSystem& other ) = delete;
		~TrenchSystem();

		void Update( const ddc::UpdateData& data, float delta_t ) override;
		void CreateRenderResources();

	private:

		ddr::MeshHandle m_chunkMesh;
		ddr::ShaderHandle m_shader;

		glm::vec3 m_trenchDirection;
		glm::vec3 m_trenchOrigin;
		DenseMap<glm::vec3, ddc::Entity> m_chunks;

		ddc::Entity CreateTrenchChunk( glm::vec3 position, ddc::World& world );
	};
}