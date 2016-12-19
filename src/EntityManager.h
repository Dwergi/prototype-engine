//
// EntityManager.h - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"
#include "Services.h"

namespace dd
{
	class EntityManager
	{
	public:

		template <typename T> struct identity { typedef T type; };

		EntityManager();
		~EntityManager();

		EntityManager( const EntityManager& ) = delete;
		EntityManager( EntityManager&& ) = delete;

		EntityManager& operator=( const EntityManager& ) = delete;
		EntityManager& operator=( EntityManager&& ) = delete;

		EntityHandle Create();
		void Destroy( const EntityHandle& entity );

		void DestroyAll();
		void Update( float dt );

		//
		// Create an entity with the given component types.
		//
		template <typename... Components>
		EntityHandle CreateEntity();

		template <typename Component>
		Component& GetComponent( EntityHandle handle ) const;

		//
		// Add a component of the given type to the entity.
		//
		template <typename Component> 
		Component* AddComponent( EntityHandle handle ) const;

		//
		// Remove a component of the given type from the entity.
		//
		template <typename Component>
		void RemoveComponent( EntityHandle handle ) const;

		//
		// Check if the given entity contains a component of the given type.
		//
		template <typename Component>
		bool HasComponent( EntityHandle handle ) const;

		//
		// Check if the given entity contains all the given components.
		//
		template <typename... Components>
		bool HasAllComponents( EntityHandle handle ) const;

		//
		// Find all entities that have all the components given.
		//
		template <typename... Components>
		Vector<EntityHandle> FindAllWith() const;

		//
		// Run the given function for all the entities that have the given component types.
		//
		template <typename... Components>
		void ForAllWith( typename identity<std::function<void( EntityHandle, Components&... )>>::type f ) const;

		BASIC_TYPE( EntityManager )

	private:

		friend class EntityHandle;

		// Flags for the state of each entity entry.
		enum EntityState : short
		{
			// Newly created or freed.
			None = 0,
		
			// Initialized, but inactive.
			Valid = 1 << 0,
		
			// Active.
			Active = 1 << 1,

			// Queued for destruction.
			Destroyed = 1 << 2
		};

		struct EntityEntry
		{
			EntityEntry( const EntityHandle& handle, short flags ) : Entity( handle ), Flags( flags ) {}

			EntityHandle Entity;
			short Flags;
		};

		enum class CommandType
		{
			Invalid,
			Create,
			Destroy
		};

		struct EntityCommand
		{
			EntityCommand( const EntityHandle& handle, CommandType type ) : Entity( handle ), Type( type ) {}

			CommandType Type;
			EntityHandle Entity;
		};

		bool m_initialized;

		Vector<int> m_free;
		Vector<EntityEntry> m_entities;
		Vector<EntityCommand> m_commands;

		int m_activeEntities;

		template <typename... Components, std::size_t... Index>
		void CreateComponents( EntityHandle handle, std::index_sequence<Index...> );

		//
		// Create a component of the specified type for the given entity.
		//
		template <typename Component, std::size_t Index>
		void CreateComponent( EntityHandle handle );

		bool IsEntityValid( const EntityHandle& entity );
	};
}

#include "EntityManager.inl"