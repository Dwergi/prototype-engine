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
				uint ID : 22;
				uint Version : 10;
			};

			uint Handle;
		};
		
		EntityHandle();
		EntityHandle( const EntityHandle& other );
		EntityHandle( EntityHandle&& other );
		EntityHandle( uint id, EntityManager& manager );

		bool IsValid() const;

		EntityHandle& operator=( EntityHandle&& other );
		EntityHandle& operator=( const EntityHandle& other );
		bool operator==( const EntityHandle& other ) const;
		bool operator!=( const EntityHandle& other ) const;

		template <typename Component>
		bool Has() const;

		template <typename Component>
		ComponentHandle<Component> Get() const;

		template <typename... Components>
		void Unpack( ComponentHandle<Components>&... args ) const;

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
		void UnpackHelper( ComponentHandle<C>& c, ComponentHandle<Rest>&... r ) const
		{
			c = Get<C>();
			UnpackHelper( r... );
		}

		void UnpackHelper() const {}
	};

	template <typename Component>
	ComponentHandle<Component> EntityHandle::Get() const
	{
		return m_manager->GetComponent<Component>( *this );
	}

	template <typename Component>
	bool EntityHandle::Has() const
	{
		return m_manager->HasComponent<Component>( *this );
	}

	template <typename... Components>
	void EntityHandle::Unpack( ComponentHandle<Components>&... args ) const
	{
		UnpackHelper( args... );
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