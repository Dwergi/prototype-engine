//
// TrenchSystem.h - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "System.h"
#include "TransformComponent.h"
#include "TrenchComponent.h"

namespace ddr
{
	struct ICamera;
}

namespace dd
{
	struct TrenchSystem : public ddc::System
	{
	public: 

		TrenchSystem();
		TrenchSystem( const TrenchSystem& other ) = delete;
		~TrenchSystem();

		void Update( const ddc::UpdateData& data ) override;
		void CreateRenderResources();

	private:

		ddr::MeshHandle m_chunkMesh;
		ddr::ShaderHandle m_shader;

		glm::vec3 m_trenchDirection;
		glm::vec3 m_trenchOrigin;
		DenseMap<glm::vec3, ddc::Entity> m_chunks;

		ddc::Entity CreateTrenchChunk( glm::vec3 position, ddc::EntityLayer& entities );
	};
}