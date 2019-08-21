//
// EntityLayer.h
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#pragma once

#include "Entity.h"
#include "MessageQueue.h"

namespace ddc
{
	static const int MAX_COMPONENTS = 64;
	static const int MAX_TAGS = 32;

	typedef std::bitset<MAX_TAGS> TagBits;
	typedef std::bitset<MAX_COMPONENTS> ComponentBits;

	//
	// An entity layer is a set of entities that can interact with each other, but not with anything in any other layers.
	// Eg. a menu might run a simulation, but it is not the same simulation as the game.
	//
	struct EntityLayer
	{
		EntityLayer(std::string name);
		~EntityLayer();

		//
		// Update the entity layer - entities are created and deleted at this point.
		//
		void Update(float delta_t);

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
		// This entity may still participate in the current frame's updates, but will be destroyed in the next Update().
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

		uint LiveCount() const { return (uint) (m_entities.size() - m_free.size()); }
		uint Size() const { return (uint) m_entities.size(); }
		 
		//
		// Get the number of components the entity has.
		//
		int ComponentCount(Entity entity) const;

		//
		// Get the component ID of the Nth component in the entity.
		//
		dd::ComponentID GetNthComponentID(Entity entity, int index) const;

		//
		// Access a component from the given entity by type ID.
		//
		void* AccessComponent( Entity entity, dd::ComponentID id ) const;

		//
		// Access the message queue of this entity layer.
		// 
		ddc::MessageQueue& Messages() { return m_messages; }
		const ddc::MessageQueue& Messages() const { return m_messages; }

		//
		// Access a component from the given entity.
		//
		template <typename T>
		T* Access( Entity entity ) const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

			return reinterpret_cast<T*>(AccessComponent( entity, type->ComponentID() ));
		}

		//
		// Access a component from the given entity.
		//
		template <typename T>
		bool Access( Entity entity, T*& cmp ) const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

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
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

			return reinterpret_cast<const T*>(GetComponent( entity, type->ComponentID() ));
		}

		void GetAllComponents( Entity entity, dd::IArray<dd::ComponentID>& components ) const;

		//
		// Does the given entity have a component of the given type ID?
		//
		bool HasComponent( Entity entity, dd::ComponentID id ) const;

		template <typename T>
		bool Has( Entity entity ) const
		{
			return HasComponent( entity, DD_FIND_TYPE( T )->ComponentID() );
		}

		//
		// Does the given entity have a component of the given type?
		//
		template <typename... TComponents>
		bool HasAll( Entity entity ) const;

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
			const dd::TypeInfo* type = DD_FIND_TYPE( T );

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
			const dd::TypeInfo* type = DD_FIND_TYPE( T );
			RemoveComponent( entity, type->ComponentID() );
		}

		//
		// Find all entities with the given type and return them in the given vector.
		//
		template <typename... TComponents>
		void FindAllWith( std::vector<Entity>& outEntities ) const
		{
			ForAllWith<TComponents>( [&outEntities]( Entity e, TComponents&... ) { outEntities.push_back( e ); } );
		}

		//
		// Find all entities with the given type IDs and return them in the given vector.
		//
		void FindAllWith( const dd::IArray<dd::ComponentID>& components, const TagBits& tags, std::vector<Entity>& outEntities ) const;


		//
		// Find all entities with the given type and return them in the given vector.
		//
		template <typename... TComponents>
		void ForAllWith( std::function<void( Entity, TComponents&... )> fn ) const;

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

		// 
		// Get the name of this entity layer.
		//
		std::string Name() const { return m_name; }

	private:

		struct EntityEntry
		{
			ddc::Entity Entity;

			union
			{
				struct
				{
					byte Alive : 1;
					byte Create : 1;
					byte Destroy : 1;
				};

				uint Flags { 0 };
			};

			ComponentBits Ownership;
			TagBits Tags;
		};

		uint m_maxEntities { 0 };

		std::vector<EntityEntry> m_entities;
		std::vector<uint> m_free;

		std::vector<byte*> m_components;

		ddc::MessageQueue m_messages;

		std::string m_name;
		uint8 m_instanceIndex { 0 };

		void UpdateStorage();
	};

	using ExpandType = int[];

	template <typename... TComponents>
	Entity EntityLayer::CreateEntity()
	{
		Entity entity = CreateEntity();
		
		ExpandType
		{
			0, (Add<TComponents>( entity ), 0)...
		};

		return entity;
	}

	template <typename T>
	void SetBitmask( ComponentBits& mask )
	{
		mask.set( DD_FIND_TYPE( T )->ComponentID(), true );
	}

	template <typename... TComponents>
	bool EntityLayer::HasAll( Entity entity ) const
	{
		ComponentBits mask;

		ExpandType
		{
			0, (SetBitmask<TComponents>( mask ), 0)...
		};

		if( IsAlive( m_entities[entity.ID] ) )
		{
			mask &= m_entities[entity.ID].Ownership;

			if( mask.count() == sizeof...(TComponents) )
			{
				return true;
			}
		}

		return false;
	}

	template <typename... TComponents>
	void EntityLayer::ForAllWith( std::function<void( Entity, TComponents&... )> fn ) const
	{
		ComponentBits mask;

		ExpandType
		{
			0, (SetBitmask<TComponents>( mask ), 0)...
		};

		DD_ASSERT( mask.any() );

		for( const EntityEntry& entry : m_entities )
		{
			if( IsAlive( entry.Entity ) )
			{
				ComponentBits entity_mask = mask;
				entity_mask &= entry.Ownership;

				if( entity_mask.count() == sizeof...( TComponents ) )
				{
					fn( entry.Entity, *Access<TComponents>( entry.Entity )... );
				}
			}
		}
	}
	
	template <typename TComponent>
	TComponent* Entity::Access() const
	{
		return Layer()->Access<TComponent>(*this);
	}

	template <typename TComponent>
	const TComponent* Entity::Get() const
	{
		return Layer()->Get<TComponent>(*this);
	}

	template <typename TComponent>
	bool Entity::Has() const
	{
		return Layer()->Has<TComponent>(*this);
	}

	template <typename TComponent>
	TComponent& Entity::Add() const
	{
		return Layer()->Add<TComponent>(*this);
	}

	template <typename TComponent>
	void Entity::Remove() const
	{
		Layer()->Remove<TComponent>(*this);
	}
}