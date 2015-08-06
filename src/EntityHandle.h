//
// EntityHandle.h - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <functional>

namespace dd
{
class EntitySystem;

	class EntityHandle
	{
	public:

		int ID;
		int Version;

		EntityHandle();

		bool IsValid() const;

		bool operator==( const EntityHandle& other ) const;
		bool operator!=( const EntityHandle& other ) const;

	private: 
	
		friend class EntitySystem;
		friend std::hash<EntityHandle>;

		static const int Invalid = -1;

		EntitySystem* m_system;

		EntityHandle( int id, int version, EntitySystem* m_system );
	};
}

// hash for STL containers
namespace std
{
	template<>
	struct hash<dd::EntityHandle>
	{
		std::size_t operator()( const dd::EntityHandle& entity ) const
		{
			return entity.ID;
		}
	};
}