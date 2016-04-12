//
// EntitySystem.h - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <queue>

#include "EntityHandle.h"
#include "Services.h"

namespace dd
{
	class EntitySystem
	{
	public:

		EntitySystem();
		~EntitySystem();

		EntityHandle Create();
		void Destroy( const EntityHandle& entity );

		void DestroyAll();
		void Update( float dt );

		BASIC_TYPE( EntitySystem )

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
			EntityEntry( const EntityHandle& handle, short flags ) : 
				Entity( handle ),
				Flags( flags )
			{
			}

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
			EntityCommand( const EntityHandle& handle, CommandType type ) :
				Entity( handle ),
				Type( type )
			{
			}

			CommandType Type;
			EntityHandle Entity;
		};

		bool m_initialized;

		Vector<int> m_free;
		Vector<EntityEntry> m_entities;
		Vector<EntityCommand> m_commands;

		int m_activeEntities;

		bool IsEntityValid( const EntityHandle& entity );
	};
}