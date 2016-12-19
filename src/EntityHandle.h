//
// EntityHandle.h - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <functional>

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
		EntityHandle( uint id, EntityManager* m_manager );

		bool IsValid() const;

		EntityHandle& operator=( const EntityHandle& other );
		bool operator==( const EntityHandle& other ) const;
		bool operator!=( const EntityHandle& other ) const;

		template <typename Component>
		bool Has() const;

		template <typename Component>
		Component& Get() const;

		template <typename... Components>
		void Unpack( Components&... args ) const;

		BEGIN_SCRIPT_STRUCT( EntityHandle )
			MEMBER( EntityHandle, Handle )
			METHOD( EntityHandle, IsValid )
		END_TYPE

	private: 
	
		friend class EntityManager;
		friend std::hash<EntityHandle>;

		EntityManager* m_manager;
		static const int Invalid = -1;

		template <typename C, typename... Rest>
		void UnpackHelper( C& c, Rest&... r ) const
		{
			c = Get<C>();
			UnpackHelper( r... );
		}

		void UnpackHelper() const {}
	};

	template <typename Component>
	Component& EntityHandle::Get() const
	{
		return m_manager->GetComponent<Component>( *this );
	}

	template <typename Component>
	bool EntityHandle::Has() const
	{
		return m_manager->HasComponent<Component>( *this );
	}

	template <typename... Components>
	void EntityHandle::Unpack( Components&... args ) const
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