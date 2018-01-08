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
#include "Stream.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"

#include <algorithm>

#include "imgui/imgui.h"

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
		SetLODLevels( DefaultLODLevels );
	}

	TerrainSystem::~TerrainSystem()
	{
		
	}

	static float DistanceTo( const glm::vec3& pos, const TerrainChunkKey& key )
	{
		glm::vec3 chunk_pos( key.X + key.Size / 2, 0, key.Y + key.Size / 2 );

		return glm::distance( pos, chunk_pos );
	}

	void TerrainSystem::ClearChunks( EntityManager& entity_manager )
	{
		for( auto& chunk : m_chunks )
		{
			chunk.second->Destroy();
		}
		
		m_chunks.clear();

		entity_manager.ForAllWithWritable<TerrainChunkComponent>( []( auto entity, auto chunk_cmp )
		{
			entity.Destroy();
		} );
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
		for( auto it : m_chunks )
		{
			it.second->RenderUpdate( it.first );
		}

		entity_manager.ForAllWithWritable<TerrainChunkComponent, MeshComponent>(
			[this]( EntityHandle entity, auto chunk_cmp, auto mesh_cmp )
		{
			RenderUpdateChunk( entity, chunk_cmp.Write(), mesh_cmp.Write() );
		} );
	}

	void TerrainSystem::RenderUpdateChunk( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp )
	{
		TerrainChunk* chunk = GetChunk( chunk_cmp->Key );
		mesh_cmp->Mesh = chunk->GetMesh();
	}

	void TerrainSystem::Update( EntityManager& entity_manager, float delta_t )
	{
		if( m_requiresRegeneration )
		{
			ClearChunks( entity_manager );

			GenerateTerrain( entity_manager );

			m_requiresRegeneration = false;
		}

		entity_manager.ForAllWithWritable<TerrainChunkComponent, MeshComponent, TransformComponent>(
			[&entity_manager, this]( EntityHandle entity, auto chunk_cmp, auto mesh_cmp, auto transform_cmp )
			{
				UpdateChunk( entity, chunk_cmp.Write(), mesh_cmp.Write(), transform_cmp.Write() );
			} );
	}

	TerrainChunk* TerrainSystem::GetChunk( const TerrainChunkKey& key )
	{
		auto it = m_chunks.find( key );
		if( it != m_chunks.end() )
		{
			return it->second;
		}

		return nullptr;
	}

	void TerrainSystem::UpdateChunk( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp, TransformComponent* transform_cmp )
	{
		if( TerrainChunk::UseDebugColours )
		{
			mesh_cmp->Colour = GetMeshColour( chunk_cmp->Key );
		}
		else
		{
			mesh_cmp->Colour = glm::vec4( 1, 1, 1, 1 );
		}
	}

	void TerrainSystem::SetOrigin( EntityHandle entity, TerrainChunkComponent* chunk_cmp, MeshComponent* mesh_cmp, TransformComponent* transform_cmp, glm::vec3 origin )
	{
		TerrainChunkKey chunk_key = chunk_cmp->Key;
		TerrainChunk* chunk = GetChunk( chunk_key );
		if( chunk == nullptr )
		{
			return;
		}

		float rounded_x = ((int) origin.x / TerrainChunk::VertexDistance) * TerrainChunk::VertexDistance;
		float rounded_z = ((int) origin.z / TerrainChunk::VertexDistance) * TerrainChunk::VertexDistance;
		glm::vec2 chunk_pos = glm::vec2( chunk_key.X + rounded_x, chunk_key.Y + rounded_z );

		chunk->SetOrigin( chunk_key, chunk_pos );

		transform_cmp->SetLocalPosition( glm::vec3( rounded_x, 0, rounded_z ) );
		transform_cmp->UpdateWorldTransform();

		mesh_cmp->UpdateBounds( transform_cmp->GetWorldTransform() );
	}

	void TerrainSystem::SaveChunkImages() const
	{
		int chunk_index = 0;

		for( auto& chunk : m_chunks )
		{
			String64 chunk_file;
			snprintf( chunk_file.data(), 64, "terrain_%d.tga", chunk_index );
			
			chunk.second->Write( chunk_file.c_str() );

			String64 chunk_normals_file;
			snprintf( chunk_normals_file.data(), 64, "terrain_%d_n.tga", chunk_index );

			chunk.second->WriteNormals( chunk_normals_file.c_str() );

			++chunk_index;
		}
	}
	
	void TerrainSystem::GenerateTerrain( EntityManager& entity_manager )
	{
		// start with a 4x4 grid of the lowest LOD level, 
		// then at each level generate the centermost 2x2 grid into a 4x4 grid of one LOD level lower
		for( int lod = 0; lod < m_lodLevels; ++lod )
		{
			GenerateLODLevel( entity_manager, lod );
		}

		// number of expected chunks
		const int halfChunks = ChunksPerDimension / 2;

		const int expected = ChunksPerDimension * ChunksPerDimension + // lod 0
			(m_lodLevels - 1) * (ChunksPerDimension * ChunksPerDimension - halfChunks * halfChunks); // rest of the LODs
		DD_ASSERT( m_chunks.size() == expected, "Wrong number of chunks generated!" );
	}

	void TerrainSystem::GenerateLODLevel( EntityManager& entity_manager, int lod )
	{
		const float vertex_distance = TerrainChunk::VertexDistance * (1 << lod);
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
				key.Size = vertex_distance;

				TerrainChunk* chunk = GenerateChunk( entity_manager, key );
				EntityHandle entity = CreateChunkEntity( entity_manager, key, chunk );

				m_chunks.insert( std::make_pair( key, chunk ) );

				++generated;
			}
		}

		const int expected = lod == 0 ? ChunksPerDimension * ChunksPerDimension : // lod 0
			(ChunksPerDimension * ChunksPerDimension - halfChunks * halfChunks); // rest of the LODs

		DD_ASSERT( generated == expected, "Wrong number of chunks generated for LOD!" );
	}

	TerrainChunk* TerrainSystem::GenerateChunk( EntityManager& entity_manager, const TerrainChunkKey& key )
	{
		DD_PROFILE_SCOPED( TerrainSystem_GenerateChunk );

		TerrainChunk* chunk = new TerrainChunk();
		chunk->Generate( key );
		chunk->SetOrigin( key, glm::vec2( 0, 0 ) );

		return chunk; 
	}

	EntityHandle TerrainSystem::CreateChunkEntity( EntityManager& entity_manager, const TerrainChunkKey& key, TerrainChunk* chunk )
	{
		DD_PROFILE_SCOPED( TerrainSystem_CreateChunkEntity );

		EntityHandle entity = entity_manager.CreateEntity<TransformComponent, MeshComponent, TerrainChunkComponent>();

		TerrainChunkComponent* chunk_cmp = entity.Get<TerrainChunkComponent>().Write();
		chunk_cmp->Key = key;
		chunk_cmp->IsActive = true;

		TransformComponent* transform_cmp = entity.Get<TransformComponent>().Write();
		transform_cmp->SetLocalPosition( glm::vec3( key.X, 0, key.Y ) );
		transform_cmp->UpdateWorldTransform();

		return entity;
	}

	void TerrainSystem::DrawDebugInternal()
	{
		ImGui::Checkbox( "Debug Colours", &TerrainChunk::UseDebugColours );

		if( ImGui::DragInt( "LODs", &m_lodLevels, 1, 1, 10 ) )
		{
			SetLODLevels( m_lodLevels );
		}

		if( ImGui::DragFloat( "Vertex Distance", &TerrainChunk::VertexDistance, 0.05f, 0.01f, 2.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::DragFloat( "Height Range", &TerrainChunk::HeightRange, 1.0f, 0.0f, 200.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::TreeNodeEx( "Amplitudes", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < TerrainChunk::Octaves; ++i )
			{
				char name[64];
				snprintf( name, 64, "Amplitude %d", i );

				if( ImGui::DragFloat( name, &TerrainChunk::Amplitudes[i], 0.01f, 0.0f, 1.0f ) )
				{
					m_requiresRegeneration = true;
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::DragFloat( "Wavelength", &TerrainChunk::Wavelength, 1.0f, 0.0f, 512.0f ) )
		{
			m_requiresRegeneration = true;
		}

		if( ImGui::Button( "Save Chunk Heightmaps" ) )
		{
			SaveChunkImages();
		}
	}
}
