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
#include "Recorder.h"
#include "PropertyList.h"

#include <iostream>
//---------------------------------------------------------------------------

Services g_services;

namespace tests
{
	const int EntityCount = 10000;
	const int FrameCount = 100;

	void profile_octree( const std::vector<EntityHandle>& handles )
	{
		Random rngEntity( 0, EntityCount );
		Random rngPosition( 0, 100 );

		TransformComponent::Pool& transform_pool = g_services.Get<TransformComponent::Pool>();
		OctreeComponent::Pool& octree_pool = g_services.Get<OctreeComponent::Pool>();

		Octree octree;
		float octree_add;
		{
			ScopedTimer timer( octree_add );

			int added = 0;

			for( const EntityHandle& handle : handles )
			{
				TransformComponent* transform_cmp = transform_pool.Find( handle );
				transform_cmp->Position = Vector4( (float) rngPosition.Next(), (float) rngPosition.Next(), (float) rngPosition.Next() );

				auto octree_cmp = octree_pool.Create( handle );
				octree_cmp->Entry = octree.Add( transform_cmp->Position );

				++added;

				ASSERT( octree.Count() == added );

				ASSERT( octree_cmp->Entry.IsValid() );
			}
		}

		std::cout << "[Octree] ADD took " << octree_add << "s for " << EntityCount << " components." << std::endl;

		float octree_find_nearest;
		{
			ScopedTimer timer( octree_find_nearest );

			std::vector<Octree::Entry> output;
			output.reserve( 50 );

			for( int i = 0; i < 100; ++i )
			{
				const EntityHandle& entity = handles[ rngEntity.Next() ];
				auto octree_cmp = octree_pool.Find( entity );
				auto transform_cmp = transform_pool.Find( entity );

				octree.GetKNearest( octree_cmp->Entry, 50, output );
			}
		}

		std::cout << "[Octree] FIND NEAREST took " << octree_find_nearest << "s for " << 100 << " components." << std::endl;

		ASSERT( octree.Count() == EntityCount );

		float octree_find_range;
		{
			ScopedTimer timer( octree_find_range );

			for( int i = 0; i < 100; ++i )
			{
				std::vector<Octree::Entry> output;
				output.reserve( 50 );

				const EntityHandle& entity = handles[ rngEntity.Next() ];
				auto octree_cmp = octree_pool.Find( entity );
				auto transform_cmp = transform_pool.Find( entity );

				octree.GetWithinRange( octree_cmp->Entry, 10, output );
			}
		}

		std::cout << "[Octree] FIND RANGE took " << octree_find_range << "s for " << 100 << " components." << std::endl;

		ASSERT( octree.Count() == EntityCount );

		float octree_remove;
		{
			ScopedTimer timer( octree_remove );

			int removed = 0;

			for( int i = 0; i < EntityCount / 2; ++i )
			{
				const EntityHandle& entity = handles[ rngEntity.Next() ];
				OctreeComponent* octree_cmp = octree_pool.Find( entity );

				if( octree_cmp->Entry.IsValid() )
				{
					octree.Remove( octree_cmp->Entry );

					octree_cmp->Entry = Octree::Entry();

					++removed;
				}
			}

			ASSERT( octree.Count() == EntityCount - removed );
		}

		std::cout << "[Octree] REMOVE took " << octree_remove << "s for " << EntityCount / 2 << " components." << std::endl;
	}
	//---------------------------------------------------------------------------

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
					ASSERT( false );
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
	//---------------------------------------------------------------------------

	void create_entities( std::vector<EntityHandle>& handles )
	{
		EntitySystem& entitySystem = g_services.Get<EntitySystem>();

		TransformComponent::Pool& transform_pool = g_services.Get<TransformComponent::Pool>();

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

		std::cout << "[DenseVector] Entity CREATE took " << entity_create << "s for " << EntityCount << " entities." << std::endl;

		//profile( transform_pool, "DenseVector", handles );

		float create;
		{
			ScopedTimer timer( create );

			for( EntityHandle handle : handles )
			{
				auto cmp = transform_pool.Create( handle );
			}
		}
		std::cout << "[DenseVector] Component CREATE took " << create << "s for " << EntityCount << " components." << std::endl;
	}
	//---------------------------------------------------------------------------

	void destroy_entities( std::vector<EntityHandle>& handles )
	{
		EntitySystem& entitySystem = g_services.Get<EntitySystem>();

		float entity_clear;
		{
			ScopedTimer timer( entity_clear );

			for( EntityHandle handle : handles )
			{
				entitySystem.DestroyEntity( handle );
			}

			entitySystem.ProcessCommands();
		}

		std::cout << "[DenseVector] Entity CLEAR took " << entity_clear << "s for " << EntityCount << " entities." << std::endl;
	}
	//---------------------------------------------------------------------------
}

int main( int argc, const char* argv[] )
{
	EntitySystem entitySystem;
	g_services.Register<EntitySystem>( &entitySystem );

	TransformComponent::Pool transform_pool;
	g_services.Register( &transform_pool );

	OctreeComponent::Pool octree_pool;
	g_services.Register( &octree_pool );

	std::vector<EntityHandle> handles;
	tests::create_entities( handles );
	tests::profile_octree( handles );
	tests::destroy_entities( handles );

	tests::RecorderTests();
	tests::FullRecorderTests();

	tests::PropertyTests();

	Vector4::RegisterType();
	TransformComponent::RegisterType();

	TransformComponent cmp;
	cmp.Position.X = 500;

	PropertyList<TransformComponent> transform_list( cmp );
	Recorder<float> x_recorder( transform_list.Find( "X" ) );

	float value = 0;
	value = x_recorder;
	ASSERT( value == 500 );

	x_recorder = 200;
	value = x_recorder;
	ASSERT( value == 200 );

	x_recorder.Undo();
	ASSERT( x_recorder == 500.f );

	ASSERT( false, "DONE!" );
	return 0;
}
//===========================================================================
