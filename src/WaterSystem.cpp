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
#include "MeshComponent.h"
#include "Plane.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"
#include "Triangulator.h"
#include "WaterComponent.h"

#include "fmt/format.h"

namespace dd
{
	ddr::ShaderHandle WaterSystem::s_shader;
	ddr::MaterialHandle WaterSystem::s_material;

	WaterSystem::WaterSystem( const TerrainParameters& params ) :
		ddc::System( "Water" ),
		m_terrainParams( params )
	{
		RequireRead<TerrainChunkComponent>( "terrain" );
		RequireRead<TransformComponent>( "terrain" );
		OptionalRead<BoundBoxComponent>( "terrain" );
		OptionalRead<BoundSphereComponent>( "terrain" );

		RequireWrite<WaterComponent>( "water" );
		RequireWrite<MeshComponent>( "water" );
		RequireRead<TransformComponent>( "water" );
	}

	static void CreateMeshVertices( dd::MeshComponent& water_mesh, dd::TerrainChunk& chunk, const ddm::Plane& water_plane )
	{
		if( !chunk.GetMesh().IsValid() )
		{
			return;
		}

		const ddr::Mesh* mesh = chunk.GetMesh().Get();

		const glm::vec3 water_height( water_plane.Parameters.w );

		dd::ConstTriangulator triangulator( *mesh );
		for( size_t i = 0; i < triangulator.Size(); ++i )
		{
			dd::ConstTriangle tri = triangulator[i];

			glm::vec3 points( tri.p0.y, tri.p1.y, tri.p2.y );

			// fully above, don't create triangles
			if( glm::all( glm::greaterThan( points, water_height ) ) )
			{
				continue;
			}
			else if( glm::all( glm::lessThan( points, water_height ) ) )
			{
				continue;
			}

			glm::vec3 hit;
			if( water_plane.IntersectsLine( tri.p0, tri.p1, hit ) )
			{
				//edges.push_back( hit );
			}

			if( water_plane.IntersectsLine( tri.p1, tri.p2, hit ) )
			{
				//edges.push_back( hit );
			}

			if( water_plane.IntersectsLine( tri.p0, tri.p2, hit ) )
			{
				//edges.push_back( hit );
			}
		}
	}

	ddc::Entity WaterSystem::CreateWaterEntity( ddc::World& world, glm::vec2 chunk_pos ) const
	{
		ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::WaterComponent, dd::BoundBoxComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );
		transform->Position = glm::vec3( chunk_pos.x, m_waterHeight, chunk_pos.y );
		transform->Update();

		dd::WaterComponent* water = world.Access<dd::WaterComponent>( entity );
		water->TerrainChunkPosition = chunk_pos;

		dd::MeshComponent* mesh_cmp = world.Access<dd::MeshComponent>( entity );
		mesh_cmp->Mesh = ddr::MeshManager::Instance()->Create( fmt::format( "water_{}x{}", chunk_pos.x, chunk_pos.y ).c_str() );

		ddr::Mesh* mesh = mesh_cmp->Mesh.Access();
		mesh->SetMaterial( s_material );

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

	static ddc::Entity FindWater( glm::vec2 pos, const ddc::DataBuffer& water_entities )
	{
		auto water_cmps = water_entities.Write<WaterComponent>();

		for( size_t i = 0; i < water_entities.Size(); ++i )
		{
			if( water_cmps[i].TerrainChunkPosition == pos )
			{
				return water_entities.Entities()[i];
			}
		}

		return ddc::Entity();
	}

	void WaterSystem::Initialize( ddc::World& world )
	{
		s_shader = ddr::ShaderManager::Instance()->Load( "mesh" );
		s_material = ddr::MaterialManager::Instance()->Create( "water" );

		ddr::Material* material = s_material.Access();
		material->SetShader( s_shader );
	}

	void WaterSystem::Update( const ddc::UpdateData& update_data )
	{	
		auto terrain = update_data.Data( "terrain" );
		auto terrain_chunks = terrain.Read<TerrainChunkComponent>();
		auto terrain_transforms = terrain.Read<TransformComponent>();
		auto terrain_bboxes = terrain.Read<BoundBoxComponent>();
		auto terrain_bspheres = terrain.Read<BoundSphereComponent>();

		auto water = update_data.Data( "water" );
		auto water_meshes = water.Write<MeshComponent>();

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

			dd::MeshComponent* water_mesh = &water_meshes[i];

			ddc::Entity water_entity = FindWater( terrain_chunk->GetPosition(), water );
			if( !water_entity.IsValid() )
			{
				water_entity = CreateWaterEntity( update_data.World(), terrain_chunk->GetPosition() );
				water_mesh = world.Access<dd::MeshComponent>( water_entity );

				++m_waterChunks;
			}

			CreateMeshVertices( *water_mesh, *terrain_chunk, local_plane );
		}
	}

	void WaterSystem::DrawDebugInternal( ddc::World& world )
	{
		ImGui::Value( "Chunks", m_waterChunks );
		ImGui::DragFloat( "Water Height", &m_waterHeight, 0.1, 0, m_terrainParams.HeightRange, "%.1f" );
	}
}