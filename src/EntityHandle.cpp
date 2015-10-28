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
		Handle( EntityHandle::Invalid )
	{

	}

	EntityHandle::EntityHandle( uint handle, EntitySystem* m_system ) :
		m_system( m_system ),
		Handle( handle )
	{

	}

	EntityHandle::EntityHandle( const EntityHandle& other ) :
		m_system( other.m_system ),
		Handle( other.Handle )
	{

	}

	bool EntityHandle::IsValid() const
	{
		return m_system->IsEntityValid( *this );
	}

	bool EntityHandle::operator==( const EntityHandle& other ) const
	{
		return Handle == other.Handle;
	}

	bool EntityHandle::operator!=( const EntityHandle& other ) const
	{
		return !(*this == other);
	}

	EntityHandle& EntityHandle::operator=( const EntityHandle& other )
	{
		m_system = other.m_system;
		Handle = other.Handle;

		return *this;
	}
}