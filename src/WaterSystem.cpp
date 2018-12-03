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
#include "Plane.h"
#include "TerrainChunk.h"
#include "TerrainChunkComponent.h"
#include "TransformComponent.h"
#include "Triangulator.h"
#include "WaterComponent.h"

namespace dd
{
	WaterSystem::WaterSystem() :
		ddc::System( "Water" )
	{
		RequireRead<TerrainChunkComponent>( "terrain" );
		RequireRead<TransformComponent>( "terrain" );
		OptionalRead<BoundBoxComponent>( "terrain" );
		OptionalRead<BoundSphereComponent>( "terrain" );

		RequireWrite<WaterComponent>();
	}

	static void CalculateWaterBorders( dd::TerrainChunk& chunk, const ddm::Plane& water_plane )
	{
		const ddr::Mesh* mesh = chunk.GetMesh().Get();

		dd::ConstTriangulator triangulator( *mesh );

		for( size_t i = 0; i < triangulator.Size(); ++i )
		{
			dd::ConstTriangle tri = triangulator[i];

			glm::vec3 normal = ddm::NormalFromTriangle( tri.p0, tri.p1, tri.p2 );
		}
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

		const ddm::Plane water_plane( glm::vec3( 0, 0, 0 ), glm::vec3( 0, -1, 0 ) );

		for( size_t i = 0; i < chunks.Size(); ++i )
		{
			ddm::AABB aabb;
			ddm::Sphere sphere;

			if( !dd::GetWorldBoundBoxAndSphere( bound_boxes.Get( i ), bound_spheres.Get( i ), transforms[i], aabb, sphere ) )
			{
				continue;
			}

			if( aabb.Min.y > 0 || aabb.Max.y < 0 )
			{
				continue;
			}

			CalculateWaterBorders( *chunks[i].Chunk, water_plane );
		}
	}
}