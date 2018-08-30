#pragma once

#include "ComponentType.h"
#include "Entity.h"
#include "FunctionView.h"
#include "MessageQueue.h"

#include <bitset>
#include <unordered_map>
#include <future>

namespace dd
{
	struct JobSystem;
}

namespace ddc
{
	struct System;
	struct SystemNode;

	struct World
	{
		World();

		void Initialize() {}
		void Shutdown() {}

		dd::MessageQueue& Messages() { return m_messages; }
		const dd::MessageQueue& Messages() const { return m_messages; }

		//
		// Create an entity. 
		// This entity will not participate in the current frame's updates, but rather be updated after the next Update().
		//
		Entity CreateEntity();

		//
		// Create an entity with the given component types.
		//
		template <typename... TComponents>
		Entity CreateEntity();

		// 
		// Destroy an entity.
		// This entity may still participate in the current frame's updates, but will be destroy in the next Update().
		//
		void DestroyEntity( Entity entity );

		//
		// Get an entity by its handle.
		//
		Entity GetEntity( uint id ) const;

		//
		// Is this entity alive?
		// This will remain true for the remainder of the current frame after Destroy() is called on this entity.
		//
		bool IsAlive( Entity entity ) const;

		//
		// Update all registered systems with the given delta.
		//
		void Update( float delta_t );

		//
		// Register a system to be updated every update.
		//
		void RegisterSystem( System& system );

		//
		// Access a component from the given entity by type ID.
		//
		void* AccessComponent( Entity entity, TypeID id ) const;

		//
		// Access a component from the given entity.
		//
		template <typename T> 
		T* AccessComponent( Entity entity ) const
		{
			return reinterpret_cast<T*>( AccessComponent( entity, T::Type.ID ) );
		}

		//
		// Get a component from the given entity.
		//
		const void* GetComponent( Entity entity, TypeID id ) const;

		//
		// Get a component from the given entity.
		//
		template <typename T>
		const T* GetComponent( Entity entity ) const
		{
			return reinterpret_cast<const T*>(GetComponent( entity, T::Type.ID ));
		}

		//
		// Does the given entity have a component of the given type ID?
		//
		bool HasComponent( Entity entity, TypeID id ) const;

		//
		// Does the given entity have a component of the given type?
		//
		template <typename T>
		bool HasComponent( Entity entity ) const
		{
			return HasComponent( entity, T::Type.ID );
		}

		//
		// Add a component to the given entity of the given type ID.
		//
		void* AddComponent( Entity entity, TypeID id );

		//
		// Add a component to the given entity of the given type.
		//
		template <typename T>
		T& AddComponent( Entity entity )
		{
			return *reinterpret_cast<T*>( AddComponent( entity, T::Type.ID ) );
		}

		//
		// Remove a component from the given entity of the given type ID.
		//
		void RemoveComponent( Entity entity, TypeID id );

		//
		// Remove a component from the given entity of the given type.
		//
		template <typename T>
		void RemoveComponent( Entity entity )
		{
			RemoveComponent( entity, T::Type.ID );
		}

		//
		// Find all entities with the given type and return them in the given vector.
		//
		template <typename T>
		void FindAllWith( std::vector<Entity>& outEntities ) const
		{
			ForAllWith<T>( [&outEntities]( Entity e, T& ) { outEntities.push_back( e ); } );
		}

		//
		// Find all entities with the given type IDs and return them in the given vector.
		//
		void FindAllWith( const dd::IArray<TypeID>& components, std::vector<Entity>& outEntities ) const;


		//
		// Find all entities with the given type and return them in the given vector.
		//
		template <typename T>
		void ForAllWith( std::function<void( Entity, T& )> fn ) const;

	private:

		int m_count { 0 };

		dd::MessageQueue m_messages;

		std::vector<Entity> m_entities;
		std::vector<int> m_free;
		std::vector<std::bitset<MAX_COMPONENTS>> m_ownership;

		std::vector<byte*> m_components;
		
		std::vector<System*> m_systems;

		void UpdateSystem( System* system, float delta_t );
		void UpdateSystemsWithTreeScheduling( std::vector<SystemNode>& systems, dd::JobSystem& jobsystem, float delta_t );
	};

	using ExpandType = int[];

	template <typename... TComponents>
	Entity World::CreateEntity()
	{
		Entity entity = CreateEntity();
		
		ExpandType
		{
			0, (AddComponent<TComponents>( entity ), 0)...
		};

		return entity;
	}

	template <typename T>
	void World::ForAllWith( std::function<void( Entity, T& )> fn ) const
	{
		std::bitset<MAX_COMPONENTS> mask;
		mask.set( T::Type.ID, true );

		for( int i = 0; i < m_count; ++i )
		{
			Entity entity = m_entities[ i ];
			if( entity.Alive )
			{
				std::bitset<MAX_COMPONENTS> entity_mask = mask;
				entity_mask &= m_ownership[ i ];

				if( entity_mask.any() )
				{
					fn( entity, *AccessComponent<T>( entity ) );
				}
			}
		}
	}
}