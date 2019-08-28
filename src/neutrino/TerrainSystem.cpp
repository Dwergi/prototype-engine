//
// TerrainSystem.cpp - Procedural terrain system.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PCH.h"
#include "neutrino/TerrainSystem.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "ICamera.h"
#include "JobSystem.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "PlayerComponent.h"
#include "Random.h"
#include "RenderData.h"
#include "Services.h"
#include "TransformComponent.h"
#include "Uniforms.h"

#include "neutrino/TerrainChunk.h"
#include "neutrino/TerrainChunkComponent.h"

#include "glm/gtc/type_ptr.hpp"

#include "fmt/format.h"

namespace neut
{
	static dd::Service<dd::JobSystem> s_jobsystem;

	static glm::vec4 GetMeshColour( glm::vec2 pos, int lod )
	{
		glm::vec4 colour( 0, 0, 0, 1 );

		int element = lod % 3;
		int intensity = lod / 3 + 1;

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

		colour[xElement] = std::abs( std::fmod( pos.x, 255.f ) ) / 255.f;
		colour[yElement] = std::abs( std::fmod( pos.y, 255.f ) ) / 255.f;
		colour[element] = 1.0f / intensity;

		return colour;
	}

	TerrainSystem::TerrainSystem() :
		ddc::System( "Terrain System" ),
		m_previousOffset( INT_MAX, INT_MAX )
	{
		RequireWrite<neut::TerrainChunkComponent>();
		RequireWrite<dd::BoundBoxComponent>();
		RequireWrite<dd::TransformComponent>();
		RequireWrite<dd::ColourComponent>();

		RequireRead<dd::TransformComponent>( "player" );
		RequireRead<dd::PlayerComponent>( "player" );

		SetPartitions( 1 );
	}

	TerrainSystem::~TerrainSystem()
	{
		
	}

	void TerrainSystem::Shutdown(ddc::EntityLayer& layer)
	{
		DestroyChunks(layer);
	}
	
	void TerrainSystem::Initialize(ddc::EntityLayer& layer)
	{
		neut::TerrainChunk::InitializeShared();
	}

	void TerrainSystem::Update( const ddc::UpdateData& update_data )
	{
		ddc::EntityLayer& layer = update_data.EntityLayer();

		if( m_requiresRegeneration )
		{
			DestroyChunks(layer);

			m_requiresRegeneration = false;
		}

		if( m_saveChunkImages )
		{
			SaveChunkImages(layer);

			m_saveChunkImages = false;
		}

		auto& chunks_data = update_data.Data();

		auto& player = update_data.Data("player");
		auto player_transforms = player.Read<dd::TransformComponent>();
		
		if (player_transforms.Size() == 0)
		{
			return;
		}

		glm::vec2 player_offset = player_transforms[0].Position.xz;

		GenerateChunks(layer, chunks_data, player_offset );

		DD_TODO( "Hmm, this means that the first generated chunks won't be updated/rendered the first frame." );

		auto entities = chunks_data.Entities();
		auto chunks = chunks_data.Write<neut::TerrainChunkComponent>();
		auto transforms = chunks_data.Write<dd::TransformComponent>();
		auto bounds = chunks_data.Write<dd::BoundBoxComponent>();
		auto colours = chunks_data.Write<dd::ColourComponent>();

		for( size_t i = 0; i < chunks.Size(); ++i )
		{
			if( m_enabled )
			{
				entities[i].AddTag( ddc::Tag::Visible );
			}
			else 
			{
				entities[i].RemoveTag( ddc::Tag::Visible );
			}

			UpdateChunk(layer, entities[ i ], chunks[ i ], bounds[ i ], transforms[ i ], colours[ i ], player_offset );
		}
	}

	int TerrainSystem::CalculateLOD( glm::vec2 chunk_pos, glm::vec2 camera_pos ) const
	{
		glm::vec2 chunk_middle = chunk_pos + glm::vec2( m_params.ChunkSize / 2 );

		float distance = glm::distance( chunk_middle, camera_pos );

		for( int i = 0; i < neut::TerrainParameters::LODs; ++i )
		{
			float f = m_params.LODSwitchDistances[ i ];
			if( distance < f )
			{
				return i;
			}
		}

		return neut::TerrainParameters::LODs - 1;
	}

	void TerrainSystem::UpdateChunk( ddc::EntityLayer& layer, ddc::Entity e, neut::TerrainChunkComponent& chunk_cmp, 
		dd::BoundBoxComponent& bounds_cmp, dd::TransformComponent& transform_cmp, 
		dd::ColourComponent& colour_cmp, glm::vec2 camera_pos )
	{
		if( m_params.UseDebugColours )
		{
			colour_cmp.Colour = GetMeshColour( chunk_cmp.Chunk->GetPosition(), chunk_cmp.Chunk->GetLOD() );
		}
		else
		{
			colour_cmp.Colour = glm::vec4( 1, 1, 1, 1 );
		}

		bounds_cmp.BoundBox = chunk_cmp.Chunk->GetBounds();

		glm::vec2 pos = chunk_cmp.Chunk->GetPosition();

		chunk_cmp.Chunk->SwitchLOD( CalculateLOD( pos, camera_pos ) );
		chunk_cmp.Chunk->Update();

		transform_cmp.Position = glm::vec3( pos.x, 0, pos.y );
		transform_cmp.Update();

		if( chunk_cmp.Chunk->GetMesh().IsValid() )
		{
			if( !e.Has<dd::MeshComponent>() )
			{
				dd::MeshComponent& mesh_cmp = e.Add<dd::MeshComponent>();
				mesh_cmp.Mesh = chunk_cmp.Chunk->GetMesh();
			}
		}
	}

	void TerrainSystem::GenerateChunks( ddc::EntityLayer& layer, const ddc::UpdateBufferView& data, glm::vec2 camera_pos )
	{
		auto chunks = data.Write<TerrainChunkComponent>();
		auto entities = data.Entities();

		std::vector<glm::vec2> required_chunks;
		required_chunks.reserve( ChunksPerDimension * ChunksPerDimension );

		glm::vec2 offset( (int) (camera_pos.x / m_params.ChunkSize), (int) (camera_pos.y / m_params.ChunkSize) );
		if( m_previousOffset == offset )
			return;

		m_previousOffset = offset;

		int half_chunks = ChunksPerDimension / 2;

		for( int y = -half_chunks; y < half_chunks; ++y )
		{
			for( int x = -half_chunks; x < half_chunks; ++x )
			{
				glm::vec2 pos;
				pos.x = offset.x * m_params.ChunkSize + x * m_params.ChunkSize;
				pos.y = offset.y * m_params.ChunkSize + y * m_params.ChunkSize;

				required_chunks.push_back( pos );
			}
		}

		DD_ASSERT( required_chunks.size() == ChunksPerDimension * ChunksPerDimension );

		std::unordered_map<glm::vec2, ddc::Entity> existing;
		existing.reserve( entities.Size() );

		for( size_t i = 0; i < chunks.Size(); ++i )
		{
			entities[i].RemoveTag( ddc::Tag::Visible );

			existing.insert( std::make_pair( chunks[ i ].Chunk->GetPosition(), entities[ i ] ) );
		}

		std::vector<glm::vec2> missing_chunks;
		missing_chunks.reserve( required_chunks.size() );

		std::vector<ddc::Entity> active;
		active.reserve( required_chunks.size() );

		for( glm::vec2 required : required_chunks )
		{
			auto it = existing.find( required );
			if( it != existing.end() )
			{
				active.push_back( it->second );
			}
			else
			{
				missing_chunks.push_back( required );
			}
		}

		for( ddc::Entity entity : active )
		{
			entity.AddTag( ddc::Tag::Visible );
		}

		for( glm::vec2 pos : missing_chunks )
		{
			int lod = CalculateLOD( pos, camera_pos );
			ddc::Entity created = CreateChunk(layer, pos, lod );
			existing.insert( std::make_pair( pos, created ) );
		}
	}

	ddc::Entity TerrainSystem::CreateChunk( ddc::EntityLayer& entities, glm::vec2 pos, int lod )
	{
		DD_PROFILE_SCOPED( TerrainSystem_CreateChunk );

		ddc::Entity& entity = entities.CreateEntity<dd::TransformComponent, neut::TerrainChunkComponent, dd::BoundBoxComponent, dd::ColourComponent>();
		entities.AddTag( entity, ddc::Tag::Visible );

		dd::ColourComponent* colour_cmp = entities.Access<dd::ColourComponent>( entity );
		colour_cmp->Colour = glm::vec4( 1 );

		dd::TransformComponent* transform_cmp = entities.Access<dd::TransformComponent>( entity );
		transform_cmp->Position = glm::vec3( pos.x, 0, pos.y );
		transform_cmp->Update();

		neut::TerrainChunkComponent* chunk_cmp = entities.Access<neut::TerrainChunkComponent>( entity );
		neut::TerrainChunk* chunk = new neut::TerrainChunk( m_params, pos );
		chunk->SwitchLOD( lod );
		chunk_cmp->Chunk = chunk;
		chunk->Update();

		return entity;
	}

	void TerrainSystem::DestroyChunks( ddc::EntityLayer& layer )
	{
		layer.ForAllWith<neut::TerrainChunkComponent>( [&layer]( ddc::Entity entity, neut::TerrainChunkComponent& chunk )
		{
			delete chunk.Chunk;
			chunk.Chunk = nullptr;
			layer.DestroyEntity( entity );
		} );
	}

	void TerrainSystem::SaveChunkImages( const ddc::EntityLayer& entities ) const
	{
		entities.ForAllWith<neut::TerrainChunkComponent>( []( ddc::Entity& entity, neut::TerrainChunkComponent& chunk )
		{
			glm::ivec2 pos = (glm::ivec2) chunk.Chunk->GetPosition();
			std::string file = fmt::format( "terrain_{}x{}_{}.tga", pos.x, pos.y, chunk.Chunk->GetLOD() );

			chunk.Chunk->WriteHeightImage( file.c_str() );
		} );
	}

	void TerrainSystem::DrawDebugInternal()
	{
		ImGui::Checkbox( "Enabled", &m_enabled );

		ImGui::Value( "Active", m_activeCount );

		ImGui::Value( "Offset", m_previousOffset );

		ImGui::Checkbox( "Debug Colours", &m_params.UseDebugColours );

		if( ImGui::TreeNodeEx( "Parameters", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			if( ImGui::DragFloat( "Chunk Size", &m_params.ChunkSize, 0.05f, 0.01f, 2.0f ) )
			{
				m_requiresRegeneration = true;
			}

			if( ImGui::DragFloat( "Height Range", &m_params.HeightRange, 1.0f, 0.0f, 200.0f ) )
			{
				m_requiresRegeneration = true;
			}

			if( ImGui::DragFloat( "Wavelength", &m_params.Noise.Wavelength, 1.0f, 1.0f, 512.0f ) )
			{
				m_requiresRegeneration = true;
			}

			if( ImGui::DragFloat( "Seed", &m_params.Noise.Seed, 0.1f, 0.0f, 512.0f ) )
			{
				m_requiresRegeneration = true;
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Height Colours", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			float previous = 0;

			for( int i = 0; i < m_params.HeightLevelCount; ++i )
			{
				std::string str = fmt::format( "Height {}", i );

				if( ImGui::TreeNodeEx( str.c_str(), ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::ColorEdit3( "Colour", glm::value_ptr( m_params.HeightColours[i] ) );
					ImGui::DragFloat( "Cutoff", &m_params.HeightCutoffs[ i ], 0.01f, previous, 1.0f );

					previous = m_params.HeightCutoffs[ i ];

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Amplitudes", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_params.Noise.MaxOctaves; ++i )
			{
				std::string str = fmt::format( "Amplitude {}", i );

				if( ImGui::DragFloat( str.c_str(), &m_params.Noise.Amplitudes[i], 0.001f, 0.0f, 1.0f ) )
				{
					m_requiresRegeneration = true;
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "LODs", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			float previous = 0;

			for( int i = 0; i < m_params.LODs; ++i )
			{
				std::string str = fmt::format( "LOD {}", i );

				if( ImGui::DragFloat( str.c_str(), &m_params.LODSwitchDistances[ i ], 1.0f, previous, 4096.0f ) )
				{
					m_requiresRegeneration = true;
				}

				previous = m_params.LODSwitchDistances[ i ];
			}

			ImGui::TreePop();
		}

		if( ImGui::Button( "Randomize" ) )
		{
			dd::RandomFloat rng( 0.0f, 1.0f );

			m_params.HeightRange = glm::mix( 0.0f, 200.0f, rng.Next() );
			m_params.Noise.Wavelength = glm::mix( 1.0f, 512.0f, rng.Next() );
			m_params.Noise.Seed = glm::mix( 0.0f, 512.0f, rng.Next() );

			float max_amplitude = 1.0f;
			for( int i = 0; i < m_params.Noise.MaxOctaves; ++i )
			{
				float amplitude = glm::mix( 0.01f, max_amplitude, rng.Next() );

				m_params.Noise.Amplitudes[i] = amplitude;

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
