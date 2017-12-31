//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "IDebugDraw.h"
#include "ISystem.h"
#include "TerrainChunkKey.h"

#include <unordered_map>

namespace dd
{
	class Camera;
	class EntityManager;
	class JobSystem;
	class ShaderProgram;
	class TerrainChunk;

	struct TerrainChunkKey;

	class TerrainSystem : public ISystem, public IDebugDraw
	{
	public:

		static const float DefaultVertexDistance;

		static const int DefaultLODLevels = 4;
		static const int ChunksPerDimension = 4;

		TerrainSystem( Camera& camera, JobSystem& jobSystem );
		~TerrainSystem();

		//
		// Set the distance in metres between vertices at the maximum resolution.
		//
		void SetVertexDistance( float distance );

		//
		// Get the distance in metres between vertices at the maximum resolution.
		//
		float GetVertexDistance() const { return m_vertexDistance; }

		void SetLODLevels( int lodLevels );

		int GetLODLevels() const { return m_lodLevels; }
		
		//
		// Initialize the terrain system. Rendering must be initialized at this point.
		//
		void Initialize( EntityManager& entityManager );
		void Update( EntityManager& entityManager, float delta_t ) override;

		//
		// Save the heightmaps of the terrain chunks generated.
		//
		void SaveChunkImages() const;

		const char* GetDebugTitle() const override { return "Terrain"; }

	protected:

		virtual void DrawDebugInternal() override;

	private:
		
		bool m_requiresRegeneration;
		float m_vertexDistance;
		int m_lodLevels;

		Camera& m_camera;
		JobSystem& m_jobSystem;
		std::unordered_map<TerrainChunkKey, TerrainChunk*> m_chunks;

		void ClearChunks( EntityManager& entityManager );

		void GenerateTerrain( EntityManager& entityManager );
		void GenerateLODLevel( EntityManager& entityManager, int lodLevel );
		TerrainChunk* GenerateChunk( EntityManager& entityManager, const TerrainChunkKey& chunk );
	};
}
