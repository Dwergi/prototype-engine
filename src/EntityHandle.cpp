//
// EntityHandle.cpp - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntityHandle.h"

#include "EntitySystem.h"

namespace dd
{
	EntityHandle::EntityHandle() :
		m_system( nullptr ),
		ID( EntityHandle::Invalid ),
		Version( EntityHandle::Invalid )
	{

	}

	EntityHandle::EntityHandle( int id, int version, EntitySystem* m_system ) :
		m_system( m_system ),
		ID( id ),
		Version( version )
	{

	}

	EntityHandle::EntityHandle( const EntityHandle& other ) :
		m_system( other.m_system ),
		ID( other.ID ),
		Version( other.Version )
	{

	}

	bool EntityHandle::IsValid() const
	{
		return m_system->IsEntityValid( *this );
	}

	bool EntityHandle::operator==( const EntityHandle& other ) const
	{
		return ID == other.ID && Version == other.Version;
	}

	bool EntityHandle::operator!=( const EntityHandle& other ) const
	{
		return !(*this == other);
	}

	EntityHandle& EntityHandle::operator=( const EntityHandle& other )
	{
		m_system = other.m_system;
		ID = other.ID;
		Version = other.Version;

		return *this;
	}
}