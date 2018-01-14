//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "ComponentHandle.h"
#include "IDebugDraw.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "MeshComponent.h"
#include "TerrainChunkKey.h"
#include "TerrainChunkComponent.h"
#include "TerrainParameters.h"
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

	class TerrainSystem : public ISystem, public IDebugDraw, public IRenderer
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
		// Initialize the terrain system.
		//
		virtual void Initialize( EntityManager& entityManager ) override;

		//
		// Update the terrain system.
		//
		virtual void Update( EntityManager& entityManager, float delta_t ) override;

		//
		// Shut down the terrain system and destroy terrain chunk meshes.
		//
		virtual void Shutdown( EntityManager& entity_manager ) override;

		//
		// Initialize render resources for the terrain system.
		//
		virtual void RenderInit( const EntityManager& entity_manager, const ICamera& camera ) override;

		//
		// Update terrain chunks on the render thread.
		//
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera ) override;

		//
		// Save the heightmaps of the terrain chunks generated.
		//
		void SaveChunkImages() const;

		//
		// The name to display in the debug view list.
		//
		const char* GetDebugTitle() const override { return "Terrain"; }

	private:
		
		bool m_requiresRegeneration;
		int m_lodLevels;

		TerrainParameters m_params;
		const ICamera& m_camera;
		JobSystem& m_jobSystem;
		std::unordered_map<TerrainChunkKey, TerrainChunk*> m_chunks;
		std::unordered_map<TerrainChunkKey, EntityHandle> m_entities;

		virtual void DrawDebugInternal() override;

		TerrainChunk* GetChunk( const TerrainChunkKey& key );

		void GenerateTerrain( EntityManager& entity_manager );
		void GenerateLODLevel( EntityManager& entity_manager, int lodLevel );

		void CreateChunk( EntityManager& entity_manager, TerrainChunkKey key );
		EntityHandle CreateChunkEntity( EntityManager& entity_manager, const TerrainChunkKey& key, TerrainChunk* chunk );

		void UpdateChunk( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp, TransformComponent* transform_cmp );

		void SetOrigin( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp, TransformComponent* transform_cmp, glm::vec3 pos );
	};
}
