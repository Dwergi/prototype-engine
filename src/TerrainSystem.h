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
	class ICamera;
	class EntityManager;
	class JobSystem;
	class ShaderProgram;
	class TerrainChunk;

	struct TerrainChunkKey;

	class TerrainSystem : public ISystem, public IDebugDraw
	{
	public:

		//
		// The default distance between vertices at LOD 0 (highest detail).
		//
		static const float DefaultVertexDistance;

		//
		// The default number of LODs to use.
		//
		static const int DefaultLODLevels = 4;

		//
		// The number of chunks we have per dimension (X/Y).
		//
		static const int ChunksPerDimension = 4;

		TerrainSystem( const ICamera& camera, JobSystem& jobSystem );
		~TerrainSystem();

		//
		// Set the distance in metres between vertices at the maximum resolution.
		//
		void SetVertexDistance( float distance );

		//
		// Get the distance in metres between vertices at the maximum resolution.
		//
		float GetVertexDistance() const { return m_vertexDistance; }

		//
		// Set the number of LOD levels to use.
		//
		void SetLODLevels( int lodLevels );

		//
		// Get the number of LOD levels in use.
		//
		int GetLODLevels() const { return m_lodLevels; }
		
		//
		// Initialize the terrain system. Rendering must be initialized at this point.
		//
		void Initialize( EntityManager& entityManager );

		//
		// Update the terrain system.
		//
		void Update( EntityManager& entityManager, float delta_t ) override;

		//
		// Save the heightmaps of the terrain chunks generated.
		//
		void SaveChunkImages() const;

		//
		// The name to display in the debug view list.
		//
		const char* GetDebugTitle() const override { return "Terrain"; }

	protected:

		virtual void DrawDebugInternal() override;

	private:
		
		bool m_requiresRegeneration;
		float m_vertexDistance;
		int m_lodLevels;

		const ICamera& m_camera;
		JobSystem& m_jobSystem;
		std::unordered_map<TerrainChunkKey, TerrainChunk*> m_chunks;

		void ClearChunks( EntityManager& entityManager );

		void GenerateTerrain( EntityManager& entityManager );
		void GenerateLODLevel( EntityManager& entityManager, int lodLevel );
		TerrainChunk* GenerateChunk( EntityManager& entityManager, const TerrainChunkKey& chunk );
	};
}
