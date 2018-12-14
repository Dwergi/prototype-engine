//
// WaterSystem.cpp
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#include "PCH.h"
#include "WaterSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "ColourComponent.h"
#include "Plane.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"
#include "Triangulator.h"
#include "WaterComponent.h"

#include "fmt/format.h"

namespace dd
{
	WaterSystem::WaterSystem( const TerrainParameters& params, dd::JobSystem& jobsystem ) :
		ddc::System( "Water" ),
		m_terrainParams( params ),
		m_jobSystem( jobsystem )
	{
		RequireRead<TerrainChunkComponent>( "terrain" );
		RequireRead<TransformComponent>( "terrain" );
		OptionalRead<BoundBoxComponent>( "terrain" );
		OptionalRead<BoundSphereComponent>( "terrain" );

		RequireWrite<WaterComponent>( "water" );
		RequireRead<TransformComponent>( "water" );

		m_noiseParams.Wavelength = 32;
	}

	ddc::Entity WaterSystem::CreateWaterEntity( ddc::World& world, glm::vec2 chunk_pos ) const
	{
		ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::WaterComponent, dd::BoundBoxComponent, dd::ColourComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );
		transform->Position = glm::vec3( chunk_pos.x, m_waterHeight, chunk_pos.y );
		transform->Update();

		dd::WaterComponent* water = world.Access<dd::WaterComponent>( entity );
		water->TerrainChunkPosition = chunk_pos;
		water->Mesh = ddr::MeshManager::Instance()->Create( fmt::format( "water_{}x{}", chunk_pos.x, chunk_pos.y ).c_str() );

		dd::ColourComponent* colour = world.Access<dd::ColourComponent>( entity );
		colour->Colour = glm::vec4( 0, 0, 1, 0.5 );

		ddr::Mesh* mesh = water->Mesh.Access();
		mesh->UseBVH( false );

		const float vertex_distance = m_terrainParams.ChunkSize / dd::WaterComponent::VertexCount;

		dd::BoundBoxComponent* bound_box = world.Access<dd::BoundBoxComponent>( entity );
		bound_box->BoundBox.Min = glm::vec3( 0 );
		bound_box->BoundBox.Max = glm::vec3( vertex_distance * dd::WaterComponent::VertexCount, 0, vertex_distance * dd::WaterComponent::VertexCount );

		const int row_width = dd::WaterComponent::VertexCount + 1;

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

	void WaterSystem::Initialize( ddc::World& world )
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

	static void PopulateWaterCache( const ddc::DataBuffer& water_entities )
	{
		auto water_cmps = water_entities.Write<WaterComponent>();
		auto entities = water_entities.Entities();

		for( size_t i = 0; i < water_entities.Size(); ++i )
		{
			glm::vec2 pos = water_cmps[ i ].TerrainChunkPosition;
			ddc::Entity entity = FindWater( pos );
			if( !entity.IsValid() )
			{
				s_waterCache.insert( std::make_pair( pos, entities[ i ] ) );
			}
		}
	}

	void WaterSystem::Update( const ddc::UpdateData& update_data )
	{
		auto terrain = update_data.Data( "terrain" );
		auto terrain_chunks = terrain.Read<TerrainChunkComponent>();
		auto terrain_transforms = terrain.Read<TransformComponent>();
		auto terrain_bboxes = terrain.Read<BoundBoxComponent>();
		auto terrain_bspheres = terrain.Read<BoundSphereComponent>();

		auto water = update_data.Data( "water" );
		PopulateWaterCache( water );

		ddc::World& world = update_data.World();

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

			dd::TerrainChunk* terrain_chunk = terrain_chunks[i].Chunk;

			ddm::Plane local_plane = water_plane.GetTransformed( terrain_transforms[i].Transform() );

			ddc::Entity water_entity = FindWater( terrain_chunk->GetPosition() );
			if( !water_entity.IsValid() )
			{
				water_entity = CreateWaterEntity( update_data.World(), terrain_chunk->GetPosition() );

				++m_waterChunks;
			}
		}
	}

	void WaterSystem::DrawDebugInternal( ddc::World& world )
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