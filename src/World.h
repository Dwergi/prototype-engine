#pragma once

#include "ComponentType.h"

#include <bitset>
#include <unordered_map>
#include <future>

namespace dd
{
	struct JobSystem;
}

namespace ddc
{
	struct World;
	struct System;

	struct Entity
	{
		friend struct World;

		Entity() :
			Handle( ~0 ),
			Flags( 0 )
		{
		}

		bool IsValid() const { return Handle != ~0; }

		union
		{
			struct
			{
				uint ID : 22;
				uint Version : 10;
			};

			uint Handle;
		};

		union
		{
			struct
			{
				byte Alive : 1;
				byte Create : 1;
				byte Destroy : 1;
			};

			uint Flags;
		};
	};

	struct World
	{
		World();

		//
		// Create an entity. 
		// This entity will not participate in the current frame's updates, but rather be updated after the next Update().
		//
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
			dd::Array<TypeID, 1> components;
			components.Add( T::Type.ID );
			FindAllWith( components, outEntities );
		}

		//
		// Find all entities with the given type IDs and return them in the given vector.
		//
		void FindAllWith( const dd::IArray<TypeID>& components, std::vector<Entity>& outEntities ) const;

	private:

		const int PARTITION_COUNT = 4;
		int m_count { 0 };

		std::vector<Entity> m_entities;
		std::vector<int> m_free;
		std::vector<std::bitset<MAX_COMPONENTS>> m_ownership;

		std::vector<byte*> m_components;
		
		std::vector<System*> m_systems;

		void UpdateSystem( System* system, float delta_t, int partition_count );
	};

	struct SystemNode
	{
		struct Edge
		{
			size_t m_from;
			size_t m_to;
		};

		System* m_system { nullptr };

		std::vector<Edge> m_out;
		std::vector<Edge> m_in;

		std::shared_future<void> m_update;
	};

	/*void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_nodes );

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_nodes );

	void UpdateSystemsWithTreeScheduling( std::vector<SystemNode>& systems, dd::JobSystem& jobsystem, World& layer );*/
}