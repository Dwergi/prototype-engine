#ifndef _ENTITYSYSTEM_H
#define _ENTITYSYSTEM_H

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

		EntityHandle CreateEntity();
		void DestroyEntity( const EntityHandle& entity );

		void DestroyAllEntities();
		void ProcessCommands();

	private:

		friend class EntityHandle;

		// Flags for the state of each entity entry.
		enum EntityState
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
			EntityEntry( const EntityHandle& handle, int flags ) : 
				Entity( handle ),
				Flags( flags )
			{
			}

			EntityHandle Entity;
			int Flags;
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

		std::queue<int> m_free;
		std::vector<EntityEntry> m_entities;
		std::vector<EntityCommand> m_commands;

		int m_activeEntities;

		bool IsEntityValid( const EntityHandle& entity );
	};
}

#endif //_ENTITYSYSTEM_H