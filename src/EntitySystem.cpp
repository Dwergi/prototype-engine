//
// EntitySystem.cpp - Entity management system.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#include "PrecompiledHeader.h"

#include "EntitySystem.h"

namespace
{
	const int Invalid = -1;
	const int Maximum = std::numeric_limits<int>::max();

	const int DefaultEntityCount = 512;
	const int DefaultCommandCount = 32;
}

EntitySystem::EntitySystem()
{

}

EntitySystem::~EntitySystem()
{

}

void EntitySystem::Initialize( const Services& services )
{
	Cleanup();

	m_initialized = true;
}

void EntitySystem::Cleanup()
{
	ProcessCommands();

	m_commands.swap( std::vector<EntityCommand>() );

	DestroyAllEntities();

	m_entities.swap( std::vector<EntityEntry>() );

	m_activeEntities = 0;

	m_free.swap( std::queue<int>() );
}

void EntitySystem::ProcessCommands()
{
	for( const EntityCommand& command : m_commands )
	{
		switch( command.Type )
		{
		case( CommandType::Create ):
			{
				EntityEntry& entry = m_entities[ command.Entity.ID ];

				entry.Flags |= EntityState::Active;

				++m_activeEntities;
			}
			break;

		case( CommandType::Destroy ):
			{
				EntityEntry& entry = m_entities[ command.Entity.ID ];

				// ensure that this entity has actually lived a full, productive life
				assert( entry.Flags & EntityState::Valid );
				assert( entry.Flags & EntityState::Active );
				assert( entry.Flags & EntityState::Destroyed );

				entry.Flags = EntityState::None;

				entry.Entity.Version += 1;

				m_free.push( entry.Entity.ID );

				--m_activeEntities;
			}
		}
	}

	m_commands.swap( std::vector<EntityCommand>() );
}

//
// Create an entity and return its handle.
// 
EntityHandle EntitySystem::CreateEntity()
{
	if( m_free.empty() )
	{
		EntityHandle handle( m_entities.size(), 0, this );

		EntityEntry entry( handle, EntityState::None );

		m_entities.push_back( entry );
		m_free.push( handle.ID );
	}

	EntityEntry& entry = m_entities[ m_free.front() ];
	m_free.pop();

	entry.Flags |= EntityState::Valid;

	EntityCommand command( entry.Entity, CommandType::Create );
	m_commands.push_back( command );

	return entry.Entity;
}

//
// Destroy an entity.
// 
void EntitySystem::DestroyEntity( const EntityHandle& handle )
{
	if( !IsEntityValid( handle ) )
		return;

	EntityEntry& entry = m_entities[ handle.ID ];
	entry.Flags |= EntityState::Destroyed;

	EntityCommand command( entry.Entity, CommandType::Destroy );

	m_commands.push_back( command );
}

void EntitySystem::DestroyAllEntities()
{
	for( EntityEntry& entry : m_entities )
	{
		DestroyEntity( entry.Entity );
	}
}

bool EntitySystem::IsEntityValid( const EntityHandle& handle )
{
	if( handle.ID == EntityHandle::Invalid )
		return false;

	if( handle.ID > (int) m_entities.size() )
		return false;

	EntityEntry& entry = m_entities[ handle.ID ];

	if( entry.Entity.Version != handle.Version )
		return false;

	if( !(entry.Flags & EntityState::Valid) )
		return false;

	if( entry.Flags & EntityState::Destroyed )
		return false;

	return true;
}