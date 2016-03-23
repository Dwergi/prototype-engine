//
// EntitySystem.cpp - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntitySystem.h"

namespace
{
	const unsigned int Invalid = -1;
	const unsigned int Maximum = ((unsigned int) -1) - 1;

	const unsigned int DefaultEntityCount = 512;
	const unsigned int DefaultCommandCount = 32;
}

namespace dd
{
	EntitySystem::EntitySystem()
	{
		m_initialized = true;
	}

	EntitySystem::~EntitySystem()
	{
		Update( 0.0f );
		DestroyAll();

		m_entities.Clear();
		m_activeEntities = 0;

		m_free.Clear();
	}

	void EntitySystem::Update( float dt )
	{
		for( const EntityCommand& command : m_commands )
		{
			switch( command.Type )
			{
			case CommandType::Create:
			{
				EntityEntry& entry = m_entities[command.Entity.ID];

				entry.Flags |= EntityState::Active;

				++m_activeEntities;
			}
									  break;

			case CommandType::Destroy:
			{
				EntityEntry& entry = m_entities[command.Entity.ID];

				// ensure that this entity has actually lived a full, productive life
				DD_ASSERT( entry.Flags & EntityState::Valid );
				DD_ASSERT( entry.Flags & EntityState::Active );
				DD_ASSERT( entry.Flags & EntityState::Destroyed );

				entry.Flags = EntityState::None;

				entry.Entity.Version += 1;

				m_free.Add( entry.Entity.ID );

				--m_activeEntities;
			}
			}
		}

		m_commands.Clear();
	}

	//
	// Create an entity and return its handle.
	// 
	EntityHandle EntitySystem::Create()
	{
		if( m_free.Size() == 0 )
		{
			EntityHandle handle;
			handle.ID = m_entities.Size();
			handle.Version = 0;
			handle.m_system = this;

			EntityEntry entry( handle, EntityState::None );

			m_entities.Add( entry );
			m_free.Add( handle.ID );
		}

		EntityEntry& entry = m_entities[m_free[0]];
		m_free.Remove( 0 );

		entry.Flags |= EntityState::Valid;

		EntityCommand command( entry.Entity, CommandType::Create );
		m_commands.Add( command );

		return entry.Entity;
	}

	//
	// Destroy an entity.
	// 
	void EntitySystem::Destroy( const EntityHandle& handle )
	{
		if( !IsEntityValid( handle ) )
			return;

		EntityEntry& entry = m_entities[handle.ID];
		entry.Flags |= EntityState::Destroyed;

		EntityCommand command( entry.Entity, CommandType::Destroy );

		m_commands.Add( command );
	}

	void EntitySystem::DestroyAll()
	{
		for( EntityEntry& entry : m_entities )
		{
			Destroy( entry.Entity );
		}
	}

	bool EntitySystem::IsEntityValid( const EntityHandle& handle )
	{
		if( handle.ID == EntityHandle::Invalid )
			return false;

		if( handle.ID > m_entities.Size() )
			return false;

		EntityEntry& entry = m_entities[handle.ID];

		if( entry.Entity.Version != handle.Version )
			return false;

		if( !(entry.Flags & EntityState::Valid) )
			return false;

		if( entry.Flags & EntityState::Destroyed )
			return false;

		return true;
	}
}