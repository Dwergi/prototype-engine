//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "FSM.h"
#include "JobSystem.h"

#include "ddm/AABB.h"

#include "ddr/Mesh.h"

#include "neutrino/TerrainParameters.h"

namespace ddr
{
	struct ICamera;
	struct UniformStorage;
}

namespace neut
{
	enum class TerrainChunkStates
	{
		InitializePending,
		InitializeDone,
		UpdatePending,
		UpdateDone,
		RenderUpdatePending,
		RenderUpdateDone,
		Ready
	};

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

		TerrainChunk(const TerrainParameters& params, glm::vec2 position);
		~TerrainChunk();

		void SwitchLOD(int lod);
		void SetNoiseOffset(glm::vec2 origin);

		void Update(dd::Job* parent_job);
		void RenderUpdate();

		void WriteHeightImage(const char* filename) const;

		ddm::AABB GetBounds() const { return m_bounds; }
		glm::vec2 GetPosition() const { return m_position; }
		int GetLOD() const { return m_lod; }

		ddr::MeshHandle GetMesh() const { return m_mesh; }

		bool IsReady() const { return m_state == TerrainChunkStates::Ready; }

	private:

		static const int MeshVertexCount = (MaxVertices + 1) * (MaxVertices + 1);
		static const int FlapVertexCount = (MaxVertices + 1) * 4;
		static const int TotalVertexCount = MeshVertexCount + FlapVertexCount;

		const TerrainParameters& m_terrainParams;

		std::atomic<bool> m_updating { false };
		dd::FSM<TerrainChunkStates> m_state;

		ddr::MeshHandle m_mesh;

		glm::vec2 m_position { 0, 0 };
		ddm::AABB m_bounds;

		std::vector<glm::vec3> m_vertices;

		glm::vec2 m_previousOffset { 0, 0 };
		glm::vec2 m_offset { 0, 0 };

		int m_lod { TerrainParameters::LODs };
		int m_minLod { TerrainParameters::LODs }; // the best quality LOD that's already been generated

		bool m_dirty { false };

		void UpdateVertices();
		void Initialize();

		void CreateMesh( glm::vec2 pos );
	};
}
