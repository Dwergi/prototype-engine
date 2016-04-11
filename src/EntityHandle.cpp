//
// EntityHandle.cpp - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntityHandle.h"

#include "EntitySystem.h"
#include "Services.h"

namespace dd
{
	EntityHandle::EntityHandle() :
		Handle( EntityHandle::Invalid ),
		m_system( nullptr )
	{

	}

	EntityHandle::EntityHandle( uint handle, EntitySystem* system ) :
		Handle( handle ),
		m_system( system )
	{

	}

	EntityHandle::EntityHandle( const EntityHandle& other ) :
		Handle( other.Handle ),
		m_system( other.m_system )
	{

	}

	bool EntityHandle::IsValid() const
	{
		return m_system != nullptr && m_system->IsEntityValid( *this );
	}

	bool EntityHandle::operator==( const EntityHandle& other ) const
	{
		return m_system == other.m_system && Handle == other.Handle;
	}

	bool EntityHandle::operator!=( const EntityHandle& other ) const
	{
		return !(*this == other);
	}

	EntityHandle& EntityHandle::operator=( const EntityHandle& other )
	{
		Handle = other.Handle;
		m_system = other.m_system;

		return *this;
	}
}