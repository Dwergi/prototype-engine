//
// EntityHandle.h - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <functional>

#include "ComponentHandle.h"

namespace dd
{
	class EntityManager;

	class EntityHandle
	{
	public:

		union
		{
			struct
			{
				int ID : 22;
				int Version : 10;
			};

			int Handle;
		};
		
		EntityHandle();
		EntityHandle( const EntityHandle& other );
		EntityHandle( EntityHandle&& other );
		EntityHandle( int id, EntityManager& manager );

		bool IsValid() const;

		EntityHandle& operator=( EntityHandle&& other );
		EntityHandle& operator=( const EntityHandle& other );
		bool operator==( const EntityHandle& other ) const;
		bool operator!=( const EntityHandle& other ) const;

		template <typename Component>
		bool Has() const;

		template <typename... Components>
		bool HasAll() const;

		template <typename Component>
		ComponentHandle<Component> Get() const;

		template <typename... Components>
		void GetAll( ComponentHandle<Components>&... args ) const;

		void Destroy();

		BEGIN_SCRIPT_STRUCT( EntityHandle )
			MEMBER( EntityHandle, Handle )
			METHOD( EntityHandle, IsValid )
		END_TYPE

	private: 
	
		friend class EntityManager;
		friend std::hash<EntityHandle>;
		template <typename C>
		friend class ComponentHandle;

		EntityManager* m_manager;
		static const int Invalid = -1;

		template <typename C, typename... Rest>
		void GetAllHelper( ComponentHandle<C>& c, ComponentHandle<Rest>&... r ) const
		{
			c = Get<C>();
			GetAllHelper( r... );
		}

		void GetAllHelper() const {}

		template <typename C = void, typename... Rest>
		bool HasAllHelper() const
		{
			bool ret = Has<C>();
			return ret && HasAllHelper<Rest...>();
		}

		template <>
		bool HasAllHelper<>() const
		{
			return true;
		}
	};

	template <typename Component>
	ComponentHandle<Component> EntityHandle::Get() const
	{
		return m_manager->GetComponent<Component>( *this );
	}

	template <typename... Components>
	void EntityHandle::GetAll( ComponentHandle<Components>&... args ) const
	{
		GetAllHelper( args... );
	}

	template <typename Component>
	bool EntityHandle::Has() const
	{
		return m_manager->HasComponent<Component>( *this );
	}

	template <typename... Components>
	bool EntityHandle::HasAll() const
	{
		return HasAllHelper<Components...>();
	}
}

// hash for STL containers
namespace std
{
	template <>
	struct hash<dd::EntityHandle>
	{
		std::size_t operator()( const dd::EntityHandle& entity ) const
		{
			return entity.ID;
		}
	};
}