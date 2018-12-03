//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "AABB.h"
#include "FSM.h"
#include "JobSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "TerrainParameters.h"

#include <atomic>

namespace ddr
{
	struct ICamera;
	struct UniformStorage;
}

namespace dd
{
	struct TerrainChunk
	{
		//
		// The number of vertices per dimension of the chunk.
		//
		static const int MaxVertices = 1 << TerrainParameters::LODs;

		//
		// Initialize resources that are shared between all chunks.
		//
		static void InitializeShared();

		//
		// Create render resources for all chunks.
		//
		static void CreateRenderResources();

		TerrainChunk( JobSystem& jobsystem, const TerrainParameters& params, glm::vec2 position );
		~TerrainChunk();
		
		void SwitchLOD( int lod );
		void SetNoiseOffset( glm::vec2 origin );

		void Update();
		void RenderUpdate();

		void WriteHeightImage( const char* filename ) const;

		ddm::AABB GetBounds() const { return m_bounds; }
		glm::vec2 GetPosition() const { return m_position; }
		int GetLOD() const { return m_lod; }
		
		ddr::MeshHandle GetMesh() const { return m_mesh; }

	private:

		static const int MeshVertexCount = (MaxVertices + 1) * (MaxVertices + 1);
		static const int FlapVertexCount = (MaxVertices + 1) * 4;
		static const int TotalVertexCount = MeshVertexCount + FlapVertexCount;

		static std::vector<uint> s_indices[ TerrainParameters::LODs ];
		static ConstBuffer<uint> s_indexBuffers[ TerrainParameters::LODs ];

		static ddr::ShaderHandle s_shader;
		static ddr::MaterialHandle s_material;

		const TerrainParameters& m_terrainParams;

		enum ChunkStates
		{
			INITIALIZE_PENDING,
			INITIALIZE_DONE,
			UPDATE_PENDING,
			UPDATE_DONE,
			RENDER_UPDATE_PENDING,
			RENDER_UPDATE_DONE,
			READY
		};

		static dd::FSMPrototype s_fsmPrototype;

		std::atomic<bool> m_updating { false };
		dd::FSM m_state;

		dd::JobSystem& m_jobsystem;
		ddr::MeshHandle m_mesh;

		glm::vec2 m_position;
		ddm::AABB m_bounds;

		std::vector<glm::vec3> m_vertices;
		ConstBuffer<glm::vec3> m_verticesBuffer;

		glm::vec2 m_previousOffset;
		glm::vec2 m_offset;

		int m_lod;
		int m_minLod { TerrainParameters::LODs }; // the best quality LOD that's already been generated

		bool m_dirty { false };

		void UpdateVertices();
		void Initialize();

		void CreateMesh( glm::vec2 pos );
	};
}
