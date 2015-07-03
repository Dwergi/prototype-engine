#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "EntitySystem.h"
#include "TransformComponent.h"
#include "OctreeComponent.h"
#include "DenseVectorPool.h"
#include "UnorderedMapPool.h"
#include "ScopedTimer.h"

const int EntityCount = 1000;
const int FrameCount = 100;

void create_handles( int count, std::vector<dd::EntityHandle>& handles, dd::EntitySystem& system )
{
	for( int i = 0; i < count; ++i )
	{
		handles.push_back( system.CreateEntity() );
	}

	system.ProcessCommands();
}

TEST_CASE( "Octree", "[octree]")
{
	dd::EntitySystem system;
	std::vector<dd::EntityHandle> handles;

	create_handles( EntityCount, handles, system );

	dd::TransformComponent::Pool transform_pool;
	dd::OctreeComponent::Pool octree_pool;

	dd::Octree octree;

	int added = 0;

	dd::Random rngPosition( 0, 100 );

	for( const dd::EntityHandle& handle : handles )
	{
		dd::TransformComponent* transform_cmp = transform_pool.Create( handle );
		transform_cmp->Position = dd::Vector4( (float) rngPosition.Next(), (float) rngPosition.Next(), (float) rngPosition.Next() );

		dd::OctreeComponent* octree_cmp = octree_pool.Create( handle );
		octree_cmp->Entry = octree.Add( transform_cmp->Position );

		++added;

		REQUIRE( octree.Count() == added );

		REQUIRE( octree_cmp->Entry.IsValid() );
	}

	std::vector<dd::Octree::Entry> output;
	output.reserve( 50 );

	dd::Random rngEntity( 0, EntityCount );

	for( int i = 0; i < 100; ++i )
	{
		const dd::EntityHandle& entity = handles[ rngEntity.Next() ];
		auto octree_cmp = octree_pool.Find( entity );
		auto transform_cmp = transform_pool.Find( entity );

		octree.GetKNearest( octree_cmp->Entry, 50, output );
	}

	REQUIRE( octree.Count() == EntityCount );

	float octree_find_range;
	{
		dd::ScopedTimer timer( octree_find_range );

		for( int i = 0; i < 100; ++i )
		{
			std::vector<dd::Octree::Entry> output;
			output.reserve( 50 );

			const dd::EntityHandle& entity = handles[ rngEntity.Next() ];
			auto octree_cmp = octree_pool.Find( entity );
			auto transform_cmp = transform_pool.Find( entity );

			octree.GetWithinRange( octree_cmp->Entry, 10, output );
		}
	}

	REQUIRE( octree.Count() == EntityCount );
		
	int removed = 0;

	for( int i = 0; i < EntityCount / 2; ++i )
	{
		const dd::EntityHandle& entity = handles[ rngEntity.Next() ];
		dd::OctreeComponent* octree_cmp = octree_pool.Find( entity );

		if( octree_cmp->Entry.IsValid() )
		{
			octree.Remove( octree_cmp->Entry );

			octree_cmp->Entry = dd::Octree::Entry();

			++removed;
		}
	}

	REQUIRE( octree.Count() == EntityCount - removed );
}
//---------------------------------------------------------------------------

TEST_CASE( "Create Entity", "[entity]" )
{
	dd::EntitySystem entitySystem;
	dd::EntityHandle handle = entitySystem.CreateEntity();

	entitySystem.ProcessCommands();

	REQUIRE( handle.IsValid() );

	SECTION( "Remove Entity" )
	{
		entitySystem.DestroyEntity( handle );
		entitySystem.ProcessCommands();

		REQUIRE( !handle.IsValid() );
	}

}
//---------------------------------------------------------------------------