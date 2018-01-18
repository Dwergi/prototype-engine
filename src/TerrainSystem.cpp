//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PrecompiledHeader.h"
#include "TerrainSystem.h"

#include "EntityManager.h"
#include "ICamera.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Random.h"
#include "Stream.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"

#include <algorithm>

#include "imgui/imgui.h"

#include "glm/gtc/type_ptr.hpp"

namespace dd
{
	static glm::vec4 GetMeshColour( const TerrainChunkKey& key )
	{
		glm::vec4 colour( 0, 0, 0, 1 );

		int element = key.LOD % 3;
		int intensity = key.LOD / 3 + 1;

		int xElement = 0;
		int yElement = 0;
		switch( element )
		{
		case 0:
			xElement = 1;
			yElement = 2;
			break;

		case 1:
			xElement = 0;
			yElement = 2;
			break;

		case 2:
			xElement = 0;
			yElement = 1;
			break;
		}

		colour[xElement] = std::abs( std::fmod( key.X, 255.f ) ) / 255.f;
		colour[yElement] = std::abs( std::fmod( key.Y, 255.f ) ) / 255.f;
		colour[element] = 1.0f / intensity;

		return colour;
	}

	TerrainSystem::TerrainSystem( const ICamera& camera, JobSystem& jobSystem ) :
		m_camera( camera ),
		m_jobSystem( jobSystem )
	{
	}

	TerrainSystem::~TerrainSystem()
	{
		
	}

	void TerrainSystem::Shutdown( EntityManager& entity_manager )
	{
		DestroyChunks( entity_manager );
	}

	void TerrainSystem::SetLODLevels( int lods )
	{
		m_lodLevels = lods;
		m_requiresRegeneration = true;
	}

	void TerrainSystem::RenderInit( const EntityManager& entity_manager, const ICamera& camera )
	{
		TerrainChunk::CreateRenderResources();
	}

	void TerrainSystem::Initialize( EntityManager& entity_manager )
	{
		TerrainChunk::InitializeShared();

		Update( entity_manager, 0 );
	}

	void TerrainSystem::Render( const EntityManager& entity_manager, const ICamera& camera )
	{
		entity_manager.ForAllWithWritable<TerrainChunkComponent, MeshComponent>(
			[&entity_manager, this]( EntityHandle entity, auto chunk_h, auto mesh_h )
		{
			TerrainChunkComponent* chunk_cmp = chunk_h.Write();
			chunk_cmp->Chunk->RenderUpdate();

			mesh_h.Write()->Mesh = chunk_cmp->Chunk->GetMesh();
		} );
	}

	void TerrainSystem::Update( EntityManager& entity_manager, float delta_t )
	{
		if( m_requiresRegeneration )
		{
			DestroyChunks( entity_manager );
			GenerateTerrain( entity_manager );

			m_requiresRegeneration = false;
		}

		if( m_saveChunkImages )
		{
			SaveChunkImages( entity_manager );

			m_saveChunkImages = false;
		}

		entity_manager.ForAllWithWritable<TerrainChunkComponent, MeshComponent>(
			[&entity_manager, this]( EntityHandle entity, auto chunk_cmp, auto mesh_cmp )
			{
				UpdateChunk( entity, chunk_cmp.Write(), mesh_cmp.Write() );
			} );
	}

	void TerrainSystem::UpdateChunk( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp )
	{
		if( m_params.UseDebugColours )
		{
			mesh_cmp->Colour = GetMeshColour( chunk_cmp->Chunk->GetKey() );
		}
		else
		{
			mesh_cmp->Colour = glm::vec4( 1, 1, 1, 1 );
		}
	}

	void TerrainSystem::SetOrigin( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp, TransformComponent* transform_cmp, glm::vec3 origin )
	{
		TerrainChunk* chunk = chunk_cmp->Chunk;
		if( chunk == nullptr )
		{
			return;
		}

		TerrainChunkKey chunk_key = chunk->GetKey();

		float rounded_x = ((int) origin.x / m_params.VertexDistance) * m_params.VertexDistance;
		float rounded_z = ((int) origin.z / m_params.VertexDistance) * m_params.VertexDistance;
		glm::vec2 chunk_pos = glm::vec2( chunk_key.X + rounded_x, chunk_key.Y + rounded_z );

		chunk->SetTerrainOrigin( chunk_pos );

		transform_cmp->SetLocalPosition( glm::vec3( rounded_x, 0, rounded_z ) );
	}
	
	void TerrainSystem::GenerateTerrain( EntityManager& entity_manager )
	{
#ifndef GENERATE_SINGLE_CHUNK

		const int expected = ChunksPerDimension * ChunksPerDimension + // lod 0
			m_lodLevels * (ChunksPerDimension * ChunksPerDimension - (ChunksPerDimension / 2) * (ChunksPerDimension / 2)); // rest of the LODs

		Vector<TerrainChunkKey> toGenerate;
		toGenerate.Reserve( expected );

		// start with a 4x4 grid of the lowest LOD level, 
		// then at each level generate the centermost 2x2 grid into a 4x4 grid of one LOD level lower
		for( int lod = 0; lod < m_lodLevels; ++lod )
		{
			GenerateLODLevel( entity_manager, lod, toGenerate );
		}

		UpdateTerrainChunks( entity_manager, toGenerate );
#else
		TerrainChunkKey key;
		key.X = 0;
		key.Y = 0;
		key.LOD = 0;

		CreateChunk( entity_manager, key );
#endif
	}

	void TerrainSystem::UpdateTerrainChunks( EntityManager& entity_manager, const Vector<TerrainChunkKey>& required_chunks )
	{
		Vector<EntityHandle>& existing_chunks = entity_manager.FindAllWithWritable<TerrainChunkComponent, MeshComponent, TransformComponent>();
		Vector<EntityHandle> still_active;
		still_active.Reserve( existing_chunks.Size() );

		Vector<TerrainChunkKey> need_creating;
		need_creating.Reserve( required_chunks.Size() );

		for( const TerrainChunkKey& required_key : required_chunks )
		{
			bool is_required = false;

			for( EntityHandle& entity : existing_chunks )
			{
				TerrainChunkComponent* terrain_chunk = entity.Get<TerrainChunkComponent>().Write();
				const TerrainChunkKey& existing_key = terrain_chunk->Chunk->GetKey();

				if( required_key == existing_key )
				{
					is_required = true;
					break;
				}
			}
		}
	}

	void TerrainSystem::GenerateLODLevel( EntityManager& entity_manager, int lod, Vector<TerrainChunkKey>& toGenerate )
	{
		const float vertex_distance = m_params.VertexDistance * (1 << lod);
		const float chunk_size = TerrainChunk::Vertices * vertex_distance;

		DD_PROFILE_SCOPED( TerrainSystem_GenerateLODLevel );

		const int halfChunks = ChunksPerDimension / 2;
		const int quarterChunks = halfChunks / 2;

		int generated = 0;

		for( int y = -halfChunks; y < halfChunks; ++y )
		{
			for( int x = -halfChunks; x < halfChunks; ++x )
			{
				// don't generate chunks for the middle-most grid unless we're at LOD 0
				if( lod != 0 &&
					(x >= -quarterChunks && x < quarterChunks) &&
					(y >= -quarterChunks && y < quarterChunks) )
				{
					continue;
				}

				TerrainChunkKey key;
				key.X = x * chunk_size;
				key.Y = y * chunk_size;
				key.LOD = lod;

				toGenerate.Add( key );

				++generated;
			}
		}

		const int expected = lod == 0 ? ChunksPerDimension * ChunksPerDimension : // lod 0
			(ChunksPerDimension * ChunksPerDimension - halfChunks * halfChunks); // rest of the LODs

		DD_ASSERT( generated != expected, "Wrong amount of chunks have been generated!" );
	}

	void TerrainSystem::CreateChunk( EntityManager& entity_manager, const TerrainChunkKey& key )
	{
		DD_PROFILE_SCOPED( TerrainSystem_CreateChunk );

		EntityHandle entity = entity_manager.CreateEntity<TransformComponent, MeshComponent, TerrainChunkComponent>();

		TerrainChunkComponent* chunk_cmp = entity.Get<TerrainChunkComponent>().Write();
		TerrainChunk* chunk = new TerrainChunk( m_params, key );
		chunk_cmp->Chunk = chunk;
		chunk_cmp->IsActive = true;

		TransformComponent* transform_cmp = entity.Get<TransformComponent>().Write();
		transform_cmp->SetLocalPosition( glm::vec3( key.X, 0, key.Y ) );

		m_jobSystem.Schedule( [this, chunk]()
		{
			chunk->Generate();
			chunk->SetTerrainOrigin( glm::vec2( 0, 0 ) );
		} );
	}

	void TerrainSystem::DestroyChunks( EntityManager& entity_manager )
	{
		entity_manager.ForAllWithWritable<TerrainChunkComponent>( []( auto entity, auto chunk_h )
		{
			TerrainChunkComponent* chunk_cmp = chunk_h.Write();
			delete chunk_cmp->Chunk;
			chunk_cmp->Chunk = nullptr;
			chunk_cmp->IsActive = false;
			entity.Destroy();
		} );
	}

	void TerrainSystem::SaveChunkImages( const EntityManager& entity_manager ) const
	{
		int chunk_index = 0;

		entity_manager.ForAllWithReadable<TerrainChunkComponent>( [&chunk_index]( EntityHandle entity, ComponentHandle<TerrainChunkComponent> chunk_h )
		{
			String64 chunk_file;
			snprintf( chunk_file.data(), 64, "terrain_%d.tga", chunk_index );

			chunk_h.Read()->Chunk->WriteHeightImage( chunk_file.c_str() );

			String64 chunk_normal_file;
			snprintf( chunk_file.data(), 64, "terrain_%d_n.tga", chunk_index );

			chunk_h.Read()->Chunk->WriteNormalImage( chunk_normal_file.c_str() );

			++chunk_index;
		} );
	}

	void TerrainSystem::DrawDebugInternal()
	{
		ImGui::Checkbox( "Debug Colours", &m_params.UseDebugColours );

		if( ImGui::DragInt( "LODs", &m_lodLevels, 1, 1, 10 ) )
		{
			SetLODLevels( m_lodLevels );
		}

		if( ImGui::DragFloat( "Vertex Distance", &m_params.VertexDistance, 0.05f, 0.01f, 2.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Height Range", &m_params.HeightRange, 1.0f, 0.0f, 200.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Wavelength", &m_params.Wavelength, 1.0f, 1.0f, 512.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Seed", &m_params.Seed, 0.1f, 0.0f, 512.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::TreeNodeEx( "Height Colours", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_params.HeightLevelCount; ++i )
			{
				char name[ 64 ];
				snprintf( name, 64, "Height %d", i );

				if( ImGui::TreeNodeEx( name, ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::ColorEdit3( "Colour", glm::value_ptr( m_params.HeightColours[i] ) );
					ImGui::DragFloat( "Cutoff", &m_params.HeightCutoffs[ i ], 0.01f, 0.0f, 1.0f );

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Amplitudes", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_params.Octaves; ++i )
			{
				char name[64];
				snprintf( name, 64, "Amplitude %d", i );

				if( ImGui::DragFloat( name, &m_params.Amplitudes[i], 0.001f, 0.0f, 1.0f ) )
				{
					m_requiresRegeneration = true;
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::Button( "Randomize" ) )
		{
			RandomFloat rng( 0.0f, 1.0f );

			m_params.Seed = glm::mix( 0.0f, 512.0f, rng.Next() );
			m_params.HeightRange = glm::mix( 0.0f, 200.0f, rng.Next() );
			m_params.Wavelength = glm::mix( 1.0f, 512.0f, rng.Next() );

			float max_amplitude = 1.0f;
			for( int i = 0; i < m_params.Octaves; ++i )
			{
				float amplitude = glm::mix( 0.01f, max_amplitude, rng.Next() );

				m_params.Amplitudes[i] = amplitude;

				max_amplitude = amplitude;
			}

			for( int i = 0; i < m_params.HeightLevelCount; ++i )
			{
				m_params.HeightColours[i] = glm::vec3( rng.Next(), rng.Next(), rng.Next() );
			}

			float previous_cutoff = 0.0f;
			for( int i = 1; i < m_params.HeightLevelCount - 1; ++i )
			{
				float cutoff = glm::mix( previous_cutoff, 1.0f, rng.Next() );

				m_params.HeightCutoffs[i] = cutoff;

				previous_cutoff = cutoff;
			}

			m_requiresRegeneration = true;
		}
		
		if( ImGui::Button( "Save Chunk Heightmaps" ) )
		{
			m_saveChunkImages = true;
		}
	}
}
