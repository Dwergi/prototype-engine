//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"
#include "TerrainChunkKey.h"
#include "TerrainParameters.h"

#include <unordered_map>

namespace ddr
{
	struct ShaderProgram;
}

namespace dd
{
	struct ICamera;
	struct JobSystem;
	struct BoundBoxComponent;
	struct ColourComponent;
	struct MeshComponent;
	class TerrainChunk;
	struct TerrainChunkComponent;
	struct TransformComponent;
	struct TerrainChunkKey;
	struct Wireframe;

	class TerrainSystem : public ddc::System, public IDebugPanel
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

		TerrainSystem( JobSystem& jobSystem );
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
		// Get the terrain parameters.
		//
		const TerrainParameters& GetTerrainParameters() const { return m_params; }
		
		//
		// Initialize the terrain system.
		//
		virtual void Initialize( ddc::World& world ) override;

		//
		// Update the terrain system.
		//
		virtual void Update( const ddc::UpdateData& data ) override;

		//
		// Shut down the terrain system and destroy terrain chunk meshes.
		//
		virtual void Shutdown( ddc::World& world ) override;

		//
		// Save the heightmaps of the terrain chunks generated.
		//
		void SaveChunkImages( const ddc::World& world ) const;

		//
		// The name to display in the debug view list.
		//
		const char* GetDebugTitle() const override { return "Terrain"; }

	private:
		
		bool m_draw { true };
		bool m_requiresRegeneration { false };
		bool m_saveChunkImages { false };
		int m_lodLevels { DefaultLODLevels };

		glm::ivec2 m_previousOffset;

		JobSystem& m_jobsystem;

		TerrainParameters m_params;

		std::unordered_map<TerrainChunkKey, ddc::Entity> m_existing;
		Vector<ddc::Entity> m_active;

		virtual void DrawDebugInternal( const ddc::World& world ) override;

		void GenerateTerrain( ddc::World& world, const ddc::DataBuffer& data, const glm::ivec2 offset );
		void GenerateLODLevel( int lodLevel, Vector<TerrainChunkKey>& toGenerate, glm::ivec2 offset );

		ddc::Entity CreateChunk( ddc::World& world, const TerrainChunkKey& key );

		void UpdateChunk( TerrainChunkComponent& chunk_cmp, MeshComponent& mesh_cmp, BoundBoxComponent& bounds_cmp, 
			TransformComponent& transform_cmp, ColourComponent& colour_cmp );
		void UpdateTerrainChunks( ddc::World& world, const ddc::DataBuffer& data, const Vector<TerrainChunkKey>& required );

		void DestroyChunks( ddc::World& world );
	};
}
