//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"
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
	struct TerrainChunk;
	struct TerrainChunkComponent;
	struct TransformComponent;
	struct TerrainChunkKey;
	struct Wireframe;

	class TerrainSystem : public ddc::System, public IDebugPanel
	{
	public:

		//
		// The number of chunks we have per dimension (X/Y).
		//
		static const int ChunksPerDimension = 32;

		TerrainSystem( JobSystem& jobSystem );
		~TerrainSystem();

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
		
		bool m_draw { false };
		bool m_requiresRegeneration { false };
		bool m_saveChunkImages { false };

		glm::vec2 m_previousOffset;

		JobSystem& m_jobsystem;

		TerrainParameters m_params;

		int m_activeCount { 0 };

		virtual void DrawDebugInternal( ddc::World& world ) override;

		ddc::Entity CreateChunk( ddc::World& world, glm::vec2 pos, int lod );

		void UpdateChunk( ddc::World& world, ddc::Entity e, TerrainChunkComponent& chunk_cmp, BoundBoxComponent& bounds_cmp, 
			TransformComponent& transform_cmp, ColourComponent& colour_cmp, glm::vec2 camera_pos );
		void GenerateChunks( ddc::World& world, const ddc::DataBuffer& data, glm::vec2 camera_pos );

		void DestroyChunks( ddc::World& world );

		int CalculateLOD( glm::vec2 chunk_pos, glm::vec2 camera_pos ) const;
	};
}
