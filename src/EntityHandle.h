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

		union
		{
			struct
			{
				uint ID : 20;
				uint Version : 12;
			};

			uint Handle;
		};
		
		EntityHandle();
		EntityHandle( const EntityHandle& other );
		EntityHandle( uint id, EntitySystem* m_system );

		bool IsValid() const;

		EntityHandle& operator=( const EntityHandle& other );
		bool operator==( const EntityHandle& other ) const;
		bool operator!=( const EntityHandle& other ) const;

		BEGIN_SCRIPT_STRUCT( EntityHandle )
			MEMBER( Handle );
			METHOD( IsValid );
		END_TYPE

	private: 
	
		friend class EntitySystem;
		friend std::hash<EntityHandle>;

		static const int Invalid = -1;

		EntitySystem* m_system;
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