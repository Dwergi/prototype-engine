//
// EntityHandle.cpp - Handles for entities. This should be the only way that entities are accessed. 
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntityHandle.h"

#include "EntityManager.h"

namespace dd
{
	EntityHandle::EntityHandle() :
		Handle( EntityHandle::Invalid ),
		m_manager( nullptr )
	{
	}

	EntityHandle::EntityHandle( int handle, EntityManager& manager ) :
		Handle( handle ),
		m_manager( &manager )
	{
	}

	EntityHandle::EntityHandle( const EntityHandle& other ) :
		Handle( other.Handle ),
		m_manager( other.m_manager )
	{
	}

	EntityHandle::EntityHandle( EntityHandle&& other ) :
		Handle( other.Handle ),
		m_manager( other.m_manager )
	{
	}

	bool EntityHandle::IsValid() const
	{
		return m_manager != nullptr && m_manager->IsEntityValid( *this );
	}

	bool EntityHandle::operator==( const EntityHandle& other ) const
	{
		return m_manager == other.m_manager && Handle == other.Handle;
	}

	bool EntityHandle::operator!=( const EntityHandle& other ) const
	{
		return !(*this == other);
	}

	EntityHandle& EntityHandle::operator=( const EntityHandle& other )
	{
		Handle = other.Handle;
		m_manager = other.m_manager;

		return *this;
	}

	EntityHandle& EntityHandle::operator=( EntityHandle&& other )
	{
		Handle = other.Handle;
		m_manager = other.m_manager;

		return *this;
	}
}