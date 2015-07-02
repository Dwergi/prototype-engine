//
// Main.cpp - Entry point into application.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"

#include "EntitySystem.h"
#include "Services.h"
#include "TransformComponent.h"
#include "ScopedTimer.h"
#include "DenseVectorPool.h"
#include "UnsortedVectorPool.h"
#include "UnorderedMapPool.h"
#include "SortedVectorPool.h"
#include "Octree.h"
#include "OctreeComponent.h"

#include <iostream>

const int EntityCount = 10000;
const int FrameCount = 100;

template< typename PoolType >
void profile( PoolType pool, const char* pool_name, std::vector<EntityHandle>& handles )
{
	float create;
	{
		ScopedTimer timer( create );

		for( EntityHandle handle : handles )
		{
			auto cmp = pool.Create( handle );
		}
	}

	std::cout << "[" << pool_name << "] Component CREATE took " << create << "s for " << EntityCount << " components." << std::endl;

	float update;
	{
		ScopedTimer timer( update );

		for( int i = 0; i < FrameCount; ++i )
		{
			int iComponentCount = 0;

			for( auto& cmp : pool )
			{
				cmp.Update( iComponentCount );

				++iComponentCount;
			}
		}
	}

	std::cout << "[" << pool_name << "] Component UPDATE took " << update << "s for " << EntityCount << " components." << std::endl;

	Random rng( 0, EntityCount - 1 );

	float find;
	{
		ScopedTimer timer( find );

		for( int i = 0; i < EntityCount; ++i )
		{
			EntityHandle handle = handles[ rng.Next() ];

			auto cmp = pool.Find( handle );
			if( cmp == nullptr )
				assert( false );
		}
	}

	std::cout << "[" << pool_name << "] Component FIND took " << find << "s for " << EntityCount << " components." << std::endl;

	float clear;
	{
		ScopedTimer timer( clear );

		for( EntityHandle handle : handles )
		{
			pool.Remove( handle );
		}
	}

	std::cout << "[" << pool_name << "] Component REMOVE took " << clear << "s for " << EntityCount << " components." << std::endl;
}

int main( int argc, const char* argv[] )
{
	EntitySystem entitySystem;

	Services services;
	services.Register<EntitySystem>( &entitySystem );

	entitySystem.Initialize( services );

	std::vector<EntityHandle> handles;

	TransformComponent::Pool transform_pool;
	OctreeComponent::Pool octree_pool;

	Random entityRNG( 0, EntityCount );

	float entity_create;
	{
		ScopedTimer timer( entity_create );

		// create twice as many as we want
		for( int i = 0; i < EntityCount * 2; ++i )
		{
			EntityHandle handle = entitySystem.CreateEntity();
			handles.push_back( handle );
		}
		entitySystem.ProcessCommands();

		// then remove roughly half
		for( int i = 0; i < EntityCount; ++i )
		{
			size_t index = (size_t) entityRNG.Next();

			auto it = handles.begin() + index;

			entitySystem.DestroyEntity( *it );

			erase_unordered( handles, it );
		}
		entitySystem.ProcessCommands();
	}

	std::cout << "Entity CREATE took " << entity_create << "s for " << EntityCount << " entities." << std::endl;

	//profile( transform_pool, "DenseVector", handles );

	Random rng( 0, 100 );
	float create;
	{
		ScopedTimer timer( create );

		for( EntityHandle handle : handles )
		{
			auto cmp = transform_pool.Create( handle );
		}
	}
	std::cout << "[DenseVector] Component CREATE took " << create << "s for " << EntityCount << " components." << std::endl;
	
	Octree octree;
	float octree_add;
	{
		ScopedTimer timer( octree_add );

		for( const EntityHandle& handle : handles )
		{
			TransformComponent* transform_cmp = transform_pool.Find( handle );
			transform_cmp->Position = Vector4( (float) rng.Next(), (float) rng.Next(), (float) rng.Next() );

			auto octree_cmp = octree_pool.Create( handle );
			octree_cmp->Entry = octree.Add( transform_cmp->Position );
		}
	}

	std::cout << "[Octree] ADD took " << octree_add << "s for " << EntityCount << " components." << std::endl;

	float octree_find_nearest;
	{
		ScopedTimer timer( octree_find_nearest );

		std::vector<Octree::Entry> output;
		output.resize( 50 );

		for( int i = 0; i < 100; ++i )
		{
			const EntityHandle& entity = handles[ entityRNG.Next() ];
			auto octree_cmp = octree_pool.Find( entity );
			auto transform_cmp = transform_pool.Find( entity );
			
			octree.GetKNearest( octree_cmp->Entry, 50, output );
		}
	}
	
	std::cout << "[Octree] FIND_NEAREST took " << octree_add << "s for " << 100 << " components." << std::endl;

	float entity_clear;
	{
		ScopedTimer timer( entity_clear );

		for( EntityHandle handle : handles )
		{
			entitySystem.DestroyEntity( handle );
		}

		entitySystem.ProcessCommands();
	}

	std::cout << "Entity CLEAR took " << entity_clear << "s for " << EntityCount << " entities." << std::endl;

	return 0;
}
