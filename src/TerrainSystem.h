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

	class TerrainSystem : ISystem
	{
	public:

		static const uint LowDetailChunkSize = 128;
		static const uint LODLevels = 4;
		static const uint LowDetailChunksPerDim = 4; // 4 per dimension
		static const uint ChunksToSplit = 4; // chunks to split at each LOD level
		static const uint MaxInactiveChunks = 8;

		TerrainSystem( Camera& camera, JobSystem& jobSystem );
		~TerrainSystem();

		//
		// Set the size in metres of each chunk.
		//
		void SetChunkSize( uint size );
		uint GetChunkSize() const { return m_chunkSize; }

		void Initialize( EntityManager& manager );
		void Update( EntityManager& manager, float delta_t );
		void Render( Camera& camera, ShaderProgram& shader );

		void SaveChunkImages() const;

		void WaitForGeneration() const;

	private:

		uint m_chunkSize;
		Camera& m_camera;
		JobSystem& m_jobSystem;
		DenseMap<TerrainChunkKey, TerrainChunk*> m_activeChunks;
		DenseMap<TerrainChunkKey, TerrainChunk*> m_inactiveChunks;

		void GenerateTerrain( const Vector<Vector<TerrainChunkKey>>& chunks, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks );
		void GenerateChunk( const TerrainChunkKey& chunk, DenseMap<TerrainChunkKey, TerrainChunk*>& activeChunks );
		void PurgeInactiveChunks();
	};
}
