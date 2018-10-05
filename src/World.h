#pragma once


#include "Entity.h"
#include "FunctionView.h"
#include "IDebugPanel.h"
#include "MessageQueue.h"
#include "SystemsSorting.h"

#include <bitset>
#include <unordered_map>
#include <future>

namespace dd
{
	struct JobSystem;
}

namespace ddc
{
	enum class Tag : uint
	{
		None = 0,
		Visible = 1,
		Focused = 2,
		Selected = 3,
		Static = 4,
		Dynamic = 5
	};

	typedef std::bitset<MAX_TAGS> TagBits;

	struct System;
	struct SystemNode;

	struct World : dd::IDebugPanel
	{
		World( dd::JobSystem& jobsystem );

		//
		// Initialize all currently registered systems.
		//
		void Initialize();

		//
		// Shut down all currently registered systems.
		//
		void Shutdown();

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
		void* AccessComponent( Entity entity, dd::ComponentID id ) const;

		//
		// Access a component from the given entity.
		//
		template <typename T>
		T* Access( Entity entity ) const
		{
			const dd::TypeInfo* type = DD_TYPE( T );

			return reinterpret_cast<T*>(AccessComponent( entity, type->ComponentID() ));
		}

		//
		// Access a component from the given entity.
		//
		template <typename T>
		bool Access( Entity entity, T*& cmp ) const
		{
			const dd::TypeInfo* type = DD_TYPE( T );

			cmp = reinterpret_cast<T*>(AccessComponent( entity, type->ComponentID() ));
			return cmp != nullptr;
		}

		//
		// Get a component from the given entity.
		//
		const void* GetComponent( Entity entity, dd::ComponentID id ) const;

		//
		// Get a component from the given entity.
		//
		template <typename T>
		const T* Get( Entity entity ) const
		{
			const dd::TypeInfo* type = DD_TYPE( T );

			return reinterpret_cast<const T*>(GetComponent( entity, type->ComponentID() ));
		}

		void GetAllComponents( Entity entity, dd::IArray<dd::ComponentID>& components ) const;

		//
		// Does the given entity have a component of the given type ID?
		//
		bool HasComponent( Entity entity, dd::ComponentID id ) const;

		//
		// Does the given entity have a component of the given type?
		//
		template <typename T>
		bool Has( Entity entity ) const
		{
			const dd::TypeInfo* type = DD_TYPE( T );

			return HasComponent( entity, type->ComponentID() );
		}

		//
		// Add a component to the given entity of the given type ID.
		//
		void* AddComponent( Entity entity, dd::ComponentID id );

		//
		// Add a component to the given entity of the given type.
		//
		template <typename T>
		T& Add( Entity entity )
		{
			const dd::TypeInfo* type = DD_TYPE( T );

			return *reinterpret_cast<T*>(AddComponent( entity, type->ComponentID() ));
		}

		//
		// Remove a component from the given entity of the given type ID.
		//
		void RemoveComponent( Entity entity, dd::ComponentID id );

		//
		// Remove a component from the given entity of the given type.
		//
		template <typename T>
		void Remove( Entity entity )
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
		void FindAllWith( const dd::IArray<dd::ComponentID>& components, const TagBits& tags, std::vector<Entity>& outEntities ) const;


		//
		// Find all entities with the given type and return them in the given vector.
		//
		template <typename T>
		void ForAllWith( std::function<void( Entity, T& )> fn ) const;

		//
		// Does the given entity have the given tag?
		//
		bool HasTag( Entity e, Tag tag ) const;

		//
		// Add the given tag to the given entity.
		//
		void AddTag( Entity e, Tag tag );

		//
		// Remove the given tag from the given entity.
		//
		void RemoveTag( Entity e, Tag tag );

		//
		// Set all of an entity's tags at once.
		//
		void SetAllTags( Entity e, TagBits tags );

		//
		// Get a copy of all the tags the entity has.
		//
		TagBits GetAllTags( Entity e ) const;

		virtual const char* GetDebugTitle() const override { return "World"; }

	private:

		struct EntityEntry
		{
			Entity Entity;
			std::bitset<MAX_COMPONENTS> Ownership;
			TagBits Tags;
		};

		uint m_count { 0 };

		dd::MessageQueue m_messages;
		dd::JobSystem& m_jobsystem;

		std::vector<EntityEntry> m_entities;
		std::vector<uint> m_free;

		std::vector<byte*> m_components;

		std::vector<System*> m_systems;
		std::vector<SystemNode> m_orderedSystems;

		bool m_drawSystemsGraph { false };

		void UpdateSystem( System* system, std::vector<std::shared_future<void>> dependencies, float delta_t );
		void UpdateSystemsWithTreeScheduling( float delta_t );

		virtual void DrawDebugInternal( const ddc::World& world ) override;
	};

	using ExpandType = int[];

	template <typename... TComponents>
	Entity World::CreateEntity()
	{
		Entity entity = CreateEntity();
		
		ExpandType
		{
			0, (Add<TComponents>( entity ), 0)...
		};

		return entity;
	}

	template <typename T>
	void World::ForAllWith( std::function<void( Entity, T& )> fn ) const
	{
		const dd::TypeInfo* type = DD_TYPE( T );
		std::bitset<MAX_COMPONENTS> mask;
		mask.set( type->ComponentID(), true );

		for( uint i = 0; i < m_count; ++i )
		{
			const EntityEntry& entry = m_entities[ i ];
			if( IsAlive( entry.Entity ) )
			{
				std::bitset<MAX_COMPONENTS> entity_mask = mask;
				entity_mask &= entry.Ownership;

				if( entity_mask.any() )
				{
					fn( entry.Entity, *Access<T>( entry.Entity ) );
				}
			}
		}
	}
}