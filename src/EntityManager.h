//
// EntityManager.h - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"
#include "DoubleBuffer.h"

namespace dd
{
	class EntityManager
	{
		template <typename T> struct identity { typedef T type; };

	public:
		EntityManager();
		~EntityManager();

		// Disable all copy and move operations.
		EntityManager( const EntityManager& ) = delete;
		EntityManager( EntityManager&& ) = delete;
		EntityManager& operator=( const EntityManager& ) = delete;
		EntityManager& operator=( EntityManager&& ) = delete;

		//
		// Create an entity and return a handle to it.
		//
		EntityHandle Create();

		//
		// Destroy the given entity.
		//
		void Destroy( EntityHandle entity );

		//
		// Destroy all entities.
		//
		void DestroyAll();

		//
		// Update all entities. 
		// This causes the write buffer to become the read buffer.
		//
		void Update( float dt );

		//
		// Create an entity with the given component types.
		//
		template <typename... Components>
		EntityHandle CreateEntity();

		//
		// Get a component handle to a component of the given type from the given entity.
		//
		template <typename Component>
		ComponentHandle<Component> GetComponent( EntityHandle handle ) const;

		//
		// Get a component handle to a component of the given type from the given entity.
		//
		template <typename Component>
		bool HasComponent( EntityHandle handle ) const;

		//
		// Add a component of the given type to the entity.
		//
		template <typename Component> 
		ComponentHandle<Component> AddComponent( EntityHandle handle ) const;

		//
		// Construct a component with the given arguments.
		//
		template <typename Component, typename... Args>
		ComponentHandle<Component> ConstructComponent( EntityHandle handle, Args&&... args ) const;

		//
		// Remove a component of the given type from the entity.
		//
		template <typename Component>
		void RemoveComponent( EntityHandle handle ) const;

		//
		// Check if the given entity contains a readable component of the given type.
		//
		template <typename Component>
		bool HasReadable( EntityHandle handle ) const;

		//
		// Check if the given entity contains a writable component of the given type.
		//
		template <typename Component>
		bool HasWritable( EntityHandle handle ) const;

		//
		// Check if the given entity contains all the given readable components.
		//
		template <typename... Components>
		bool HasAllReadable( EntityHandle handle ) const;

		//
		// Check if the given entity contains all the given writable components.
		//
		template <typename... Components>
		bool HasAllWritable( EntityHandle handle ) const;

		//
		// Find all entities that have all the readable components given.
		//
		template <typename... Components>
		Vector<EntityHandle> FindAllWithReadable() const;

		//
		// Find all entities that have all the writable components given.
		//
		template <typename... Components>
		Vector<EntityHandle> FindAllWithWritable() const;

		//
		// Run the given function for all the entities that have the given readable component types.
		//
		template <typename... Components>
		void ForAllWithReadable( typename identity<std::function<void( EntityHandle, ComponentHandle<Components>... )>>::type f ) const;

		//
		// Run the given function for all the entities that have the given writable component types.
		//
		template <typename... Components>
		void ForAllWithWritable( typename identity<std::function<void( EntityHandle, ComponentHandle<Components>... )>>::type f ) const;

		//
		// Get a readable component of the given type for the handle h.
		//
		template <typename Component>
		const Component* GetReadable( EntityHandle h ) const;

		//
		// Get a writable component of the given type for the handle h.
		//
		template <typename Component>
		Component* GetWritable( EntityHandle h ) const;

		//
		// Register the given component type.
		//
		template <typename Component>
		void RegisterComponent();

		BASIC_TYPE( EntityManager )

	private:

		friend class EntityHandle;

		std::recursive_mutex m_mutex;
		bool m_initialized { false };

		DoubleBuffer<Vector<int>> m_free;
		DoubleBuffer<Vector<EntityHandle>> m_entities;

		DenseMap<uint64, DoubleBufferBase*> m_pools;

		template <typename... Components, std::size_t... Index>
		void CreateComponents( EntityHandle handle, std::index_sequence<Index...> );

		//
		// Create a component of the specified type for the given entity.
		//
		template <typename Component, std::size_t Index>
		void CreateComponent( EntityHandle handle );

		bool IsEntityValid( EntityHandle entity );

		template <typename Component>
		DoubleBuffer<typename Component::Pool>* GetPool() const;
	};
}

#include "EntityManager.inl"