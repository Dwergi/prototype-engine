//
// WaterSystem.cpp
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#include "PCH.h"
#include "neutrino/WaterSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "ColourComponent.h"
#include "Plane.h"
#include "Services.h"
#include "TransformComponent.h"
#include "Triangulator.h"

#include "neutrino/TerrainChunk.h"
#include "neutrino/TerrainChunkComponent.h"
#include "neutrino/WaterComponent.h"

#include <fmt/format.h>

namespace neut
{
	static dd::Service<dd::JobSystem> s_jobsystem;
	static dd::Service<ddr::MeshManager> s_meshManager;

	WaterSystem::WaterSystem( const TerrainParameters& params ) :
		ddc::System( "Water" ),
		m_terrainParams( params )
	{
		RequireRead<neut::TerrainChunkComponent>( "terrain" );
		RequireRead<dd::TransformComponent>( "terrain" );
		OptionalRead<dd::BoundBoxComponent>( "terrain" );
		OptionalRead<dd::BoundSphereComponent>( "terrain" );

		RequireWrite<neut::WaterComponent>( "water" );
		RequireRead<dd::TransformComponent>( "water" );

		m_noiseParams.Wavelength = 32;
	}

	ddc::Entity WaterSystem::CreateWaterEntity( ddc::EntityLayer& entities, glm::vec2 chunk_pos ) const
	{
		ddc::Entity entity = entities.CreateEntity<dd::TransformComponent, neut::WaterComponent, dd::BoundBoxComponent, dd::ColourComponent>();
		entities.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform = entities.Access<dd::TransformComponent>( entity );
		transform->Position = glm::vec3( chunk_pos.x, m_waterHeight, chunk_pos.y );
		transform->Update();

		neut::WaterComponent* water = entities.Access<neut::WaterComponent>( entity );
		water->TerrainChunkPosition = chunk_pos;

		std::string mesh_name = fmt::format( "water_{}x{}", chunk_pos.x, chunk_pos.y );
		water->Mesh = s_meshManager->Create( mesh_name.c_str() );

		ddr::Mesh* mesh = water->Mesh.Access();
		mesh->SetMaterial( ddr::MaterialHandle("water") );
		mesh->UseBVH( false );

		dd::ColourComponent* colour = entities.Access<dd::ColourComponent>( entity );
		colour->Colour = glm::vec4( 0, 0, 1, 0.5 );

		const float vertex_distance = m_terrainParams.ChunkSize / neut::WaterComponent::VertexCount;

		dd::BoundBoxComponent* bound_box = entities.Access<dd::BoundBoxComponent>( entity );
		bound_box->BoundBox.Min = glm::vec3( 0 );
		bound_box->BoundBox.Max = glm::vec3( vertex_distance * neut::WaterComponent::VertexCount, 0, vertex_distance * neut::WaterComponent::VertexCount );

		const int row_width = neut::WaterComponent::VertexCount + 1;

		water->Vertices.resize( row_width * row_width );

		for( int z = 0; z < row_width; ++z )
		{
			for( int x = 0; x < row_width; ++x )
			{
				water->Vertices[z * row_width + x] = glm::vec3( x * vertex_distance, 0, z * vertex_distance );
			}
		}

		water->Dirty = true;

		return entity;
	}	

	void WaterSystem::Initialize( ddc::EntityLayer& entities )
	{
	}

	static std::unordered_map<glm::vec2, ddc::Entity> s_waterCache;

	static ddc::Entity FindWater( glm::vec2 pos )
	{
		auto it = s_waterCache.find( pos );
		if( it == s_waterCache.end() )
		{
			return ddc::Entity();
		}

		return it->second;
	}

	static void PopulateWaterCache( const ddc::UpdateBufferView& water_entities )
	{
		s_waterCache.clear();

		auto water_cmps = water_entities.Write<WaterComponent>();
		auto entities = water_entities.Entities();

		for( size_t i = 0; i < water_entities.Size(); ++i )
		{
			glm::vec2 pos = water_cmps[ i ].TerrainChunkPosition;
			s_waterCache.insert( std::make_pair( pos, entities[ i ] ) );
		}
	}

	void WaterSystem::Update( const ddc::UpdateData& update_data )
	{
		auto& terrain = update_data.Data( "terrain" );
		auto terrain_chunks = terrain.Read<neut::TerrainChunkComponent>();
		auto terrain_transforms = terrain.Read<dd::TransformComponent>();
		auto terrain_bboxes = terrain.Read<dd::BoundBoxComponent>();
		auto terrain_bspheres = terrain.Read<dd::BoundSphereComponent>();

		auto& water = update_data.Data( "water" );
		PopulateWaterCache( water );

		ddc::EntityLayer& entities = update_data.EntityLayer();

		const ddm::Plane water_plane( glm::vec3( 0, m_waterHeight, 0 ), glm::vec3( 0, -1, 0 ) );

		for( size_t i = 0; i < terrain_chunks.Size(); ++i )
		{
			ddm::AABB aabb;
			ddm::Sphere sphere;

			if( !dd::GetWorldBoundBoxAndSphere( terrain_bboxes.Get( i ), terrain_bspheres.Get( i ), terrain_transforms[i], aabb, sphere ) )
			{
				continue;
			}

			if( aabb.Min.y > m_waterHeight )
			{
				continue;
			}

			neut::TerrainChunk* terrain_chunk = terrain_chunks[i].Chunk;

			ddm::Plane local_plane = water_plane.GetTransformed( terrain_transforms[i].Transform() );

			ddc::Entity water_entity = FindWater( terrain_chunk->GetPosition() );
			if( !water_entity.IsValid() )
			{
				water_entity = CreateWaterEntity( update_data.EntityLayer(), terrain_chunk->GetPosition() );

				++m_waterChunks;
			}
		}
	}

	void WaterSystem::DrawDebugInternal()
	{
		ImGui::Value( "Chunks", m_waterChunks );
		ImGui::DragFloat( "Water Height", &m_waterHeight, 0.1, 0, m_terrainParams.HeightRange, "%.1f" );

		ImGui::DragFloat( "Wave Height", &m_waveHeight, 0.1, 0, 10, "%.1f" );
		ImGui::DragFloat2( "Lateral Waves", glm::value_ptr( m_waveLateral ), 0.01, 0, 5, "%.2f" );

		if( ImGui::TreeNodeEx( "Noise", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			if( ImGui::DragFloat( "Wavelength", &m_noiseParams.Wavelength, 1.0f, 1.0f, 512.0f ) )
			{
				m_regenerate = true;
			}

			if( ImGui::DragFloat( "Seed", &m_noiseParams.Seed, 0.1f, 0.0f, 512.0f ) )
			{
				m_regenerate = true;
			}

			if( ImGui::TreeNodeEx( "Amplitudes", ImGuiTreeNodeFlags_CollapsingHeader ) )
			{
				for( int i = 0; i < m_noiseParams.MaxOctaves; ++i )
				{
					std::string str = fmt::format( "Amplitude {}", i );

					if( ImGui::DragFloat( str.c_str(), &m_noiseParams.Amplitudes[i], 0.001f, 0.0f, 1.0f ) )
					{
						m_regenerate = true;
					}
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

	}
}