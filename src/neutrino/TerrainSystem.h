//
// TerrainSystem.h - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"

#include "neutrino/TerrainParameters.h"

namespace ddr
{
	struct Shader;
}

namespace dd
{
	struct BoundBoxComponent;
	struct ColourComponent;
	struct MeshComponent;
	struct TransformComponent;
	struct Wireframe;
}

namespace neut
{
	struct TerrainChunk;
	struct TerrainChunkComponent;
	struct TerrainChunkKey;

	struct TerrainSystem : ddc::System, dd::IDebugPanel
	{
		//
		// The number of chunks we have per dimension (X/Y).
		//
		static const int ChunksPerDimension = 32;

		TerrainSystem();
		~TerrainSystem();

		//
		// Get the terrain parameters.
		//
		const TerrainParameters& GetTerrainParameters() const { return m_params; }
		
		//
		// Initialize the terrain system.
		//
		virtual void Initialize( ddc::EntitySpace& entities ) override;

		//
		// Update the terrain system.
		//
		virtual void Update( const ddc::UpdateData& data ) override;

		//
		// Shut down the terrain system and destroy terrain chunk meshes.
		//
		virtual void Shutdown( ddc::EntitySpace& entities ) override;

		//
		// Save the heightmaps of the terrain chunks generated.
		//
		void SaveChunkImages( const ddc::EntitySpace& entities ) const;

		//
		// The name to display in the debug view list.
		//
		const char* GetDebugTitle() const override { return "Terrain"; }

	private:
		
		bool m_enabled { true };
		bool m_requiresRegeneration { false };
		bool m_saveChunkImages { false };

		glm::vec2 m_previousOffset { 0, 0 };

		neut::TerrainParameters m_params;

		int m_activeCount { 0 };

		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;

		ddc::Entity CreateChunk( ddc::EntitySpace& entities, glm::vec2 pos, int lod );

		void UpdateChunk( ddc::EntitySpace& entities, ddc::Entity e, neut::TerrainChunkComponent& chunk_cmp, 
			dd::BoundBoxComponent& bounds_cmp, dd::TransformComponent& transform_cmp, 
			dd::ColourComponent& colour_cmp, glm::vec2 camera_pos );

		void GenerateChunks( ddc::EntitySpace& entities, const ddc::UpdateDataBuffer& data, glm::vec2 camera_pos );

		void DestroyChunks( ddc::EntitySpace& entities );

		int CalculateLOD( glm::vec2 chunk_middle, glm::vec2 camera_pos ) const;
	};
}
