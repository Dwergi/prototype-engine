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

namespace dd
{
	const float WATER_HEIGHT = 32.0f;

	WaterSystem::WaterSystem() :
		ddc::System( "Water" )
	{
		RequireRead<TerrainChunkComponent>( "terrain" );
		RequireRead<TransformComponent>( "terrain" );
		OptionalRead<BoundBoxComponent>( "terrain" );
		OptionalRead<BoundSphereComponent>( "terrain" );

		RequireWrite<WaterComponent>();
		RequireRead<TransformComponent>();
	}

	static void CalculateWaterBorders( dd::TerrainChunk& chunk, const ddm::Plane& water_plane, std::vector<glm::vec3>& edges )
	{
		if( !chunk.GetMesh().IsValid() )
		{
			return;
		}

		const ddr::Mesh* mesh = chunk.GetMesh().Get();

		dd::ConstTriangulator triangulator( *mesh );

		for( size_t i = 0; i < triangulator.Size(); ++i )
		{
			dd::ConstTriangle tri = triangulator[i];

			glm::vec3 hit;
			if( water_plane.IntersectsLine( tri.p0, tri.p1, hit ) )
			{
				edges.push_back( hit );
			}

			if( water_plane.IntersectsLine( tri.p1, tri.p2, hit ) )
			{
				edges.push_back( hit );
			}

			if( water_plane.IntersectsLine( tri.p0, tri.p2, hit ) )
			{
				edges.push_back( hit );
			}
		}
	}

	static void CreateWaterMesh( ddc::World& world, glm::vec2 chunk, std::vector<glm::vec3>& edges )
	{
		//world.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::WaterComponent>();
	}

	void WaterSystem::Update( const ddc::UpdateData& update_data )
	{
		auto terrain = update_data.Data( "terrain" );
		
		auto chunks = terrain.Read<TerrainChunkComponent>();
		auto transforms = terrain.Read<TransformComponent>();
		auto bound_boxes = terrain.Read<BoundBoxComponent>();
		auto bound_spheres = terrain.Read<BoundSphereComponent>();

		auto water = update_data.Data();
		auto water_transforms = water.Read<TransformComponent>();
		auto water_cmps = water.Write<WaterComponent>();
		
		const ddm::Plane water_plane( glm::vec3( 0, WATER_HEIGHT, 0 ), glm::vec3( 0, -1, 0 ) );

		std::vector<glm::vec3> edges;

		for( size_t i = 0; i < chunks.Size(); ++i )
		{
			edges.clear();

			ddm::AABB aabb;
			ddm::Sphere sphere;

			if( !dd::GetWorldBoundBoxAndSphere( bound_boxes.Get( i ), bound_spheres.Get( i ), transforms[i], aabb, sphere ) )
			{
				continue;
			}

			if( aabb.Min.y > WATER_HEIGHT || aabb.Max.y < WATER_HEIGHT )
			{
				continue;
			}

			ddm::Plane local_plane = water_plane.GetTransformed( transforms[i].Transform() );

			CalculateWaterBorders( *chunks[i].Chunk, local_plane, edges );

			if( !edges.empty() )
			{
				CreateWaterMesh( update_data.World(), chunks[i].Chunk->GetPosition(), edges );
			}
		}
	}
}