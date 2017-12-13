//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "ISystem.h"

namespace dd
{
	class Camera;
	class EntityManager;
	class JobSystem;
	class ShaderProgram;
	class TerrainChunk;

	struct TerrainChunkKey;

	class TerrainSystem : public ISystem
	{
	public:

		static const int LowDetailChunkSize = 128;
		static const int LODLevels = 4;
		static const int LowDetailChunksPerDim = 4; // 4 per dimension
		static const int ChunksToSplit = 4; // chunks to split at each LOD level
		static const int MaxInactiveChunks = 8;

		TerrainSystem( Camera& camera, JobSystem& jobSystem );
		~TerrainSystem();

		//
		// Set the size in metres of each chunk.
		//
		void SetChunkSize( int size ) { m_chunkSize = size; }
		int GetChunkSize() const { return m_chunkSize; }

		void Initialize( EntityManager& entity_manager );
		void Update( EntityManager& entity_manager, float delta_t );

		void SaveChunkImages() const;

		void WaitForGeneration() const;

	private:
		
		int m_chunkSize;
		Camera& m_camera;
		JobSystem& m_jobSystem;
		DenseMap<TerrainChunkKey, TerrainChunk*> m_activeChunks;
		DenseMap<TerrainChunkKey, TerrainChunk*> m_inactiveChunks;

		void GenerateTerrain( EntityManager& entity_manager, const Vector<Vector<TerrainChunkKey>>& chunks, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks );
		void GenerateChunk( EntityManager& entity_manager, const TerrainChunkKey& chunk, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks );
		void PurgeInactiveChunks( EntityManager& entity_manager );
	};
}
