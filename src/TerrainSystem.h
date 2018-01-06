//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "ComponentHandle.h"
#include "IDebugDraw.h"
#include "ISystem.h"
#include "MeshComponent.h"
#include "TerrainChunkKey.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"

#include <unordered_map>

namespace dd
{
	class ICamera;
	class EntityManager;
	class JobSystem;
	class MeshComponent;
	class ShaderProgram;
	class TerrainChunk;
	class TerrainChunkComponent;
	class TransformComponent;

	struct TerrainChunkKey;

	class TerrainSystem : public ISystem, public IDebugDraw
	{
	public:

		//
		// The default number of LODs to use.
		//
		static const int DefaultLODLevels = 6;

		//
		// The number of chunks we have per dimension (X/Y).
		//
		static const int ChunksPerDimension = 4;

		TerrainSystem( const ICamera& camera, JobSystem& jobSystem );
		~TerrainSystem();

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
		int m_lodLevels;

		const ICamera& m_camera;
		JobSystem& m_jobSystem;
		std::unordered_map<TerrainChunkKey, TerrainChunk*> m_chunks;
		std::unordered_map<TerrainChunkKey, EntityHandle> m_entities;

		void GenerateTerrain( EntityManager& entityManager );
		void GenerateLODLevel( EntityManager& entityManager, int lodLevel );
		TerrainChunk* GenerateChunk( EntityManager& entityManager, const TerrainChunkKey& chunk );

		EntityHandle CreateChunkEntity( EntityManager& entityManager, const TerrainChunkKey& key, TerrainChunk* chunk );
		void UpdateChunk( EntityHandle entity, ComponentHandle<TerrainChunkComponent> chunk_cmp, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp );

		void ClearChunks( EntityManager& entityManager );

		void SetOrigin( EntityHandle entity, ComponentHandle<TerrainChunkComponent> chunk_cmp, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp, glm::vec3 pos );

		TerrainChunk* GetChunk( const TerrainChunkKey& key );
	};
}
