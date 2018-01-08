//
// EntityManager.cpp - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntityManager.h"

#include "ComponentPoolBase.h"

#pragma optimize( "", off )

namespace
{
	const unsigned int Maximum = ((unsigned int) -1) - 1;

	const unsigned int DefaultEntityCount = 512;
	const unsigned int DefaultCommandCount = 32;
}

namespace dd
{
	EntityManager::EntityManager() :
		m_free( new Vector<int>(), new Vector<int>() ),
		m_entities( new Vector<EntityHandle>(), new Vector<EntityHandle>() )
	{
		m_initialized = true;
	}

	EntityManager::~EntityManager()
	{
		Update( 0.0f );
		DestroyAll();

		m_entities.Clear();
		m_free.Clear();

		for( auto it : m_pools )
		{
			delete it.Value;
		}

		m_pools.Clear();
	}

	void EntityManager::Update( float dt )
	{
		m_entities.Swap();
		m_entities.Duplicate();

		m_free.Swap();
		m_free.Duplicate();

		for( auto it : m_pools )
		{
			it.Value->Swap();
			it.Value->Duplicate();
		}
	}

	//
	// Create an entity and return its handle.
	// 
	EntityHandle EntityManager::Create()
	{
		if( m_free.GetWrite().Size() == 0 )
		{
			EntityHandle handle;
			handle.ID = m_entities.GetWrite().Size();
			handle.Version = 0;
			handle.m_manager = this;

			m_entities.GetWrite().Add( handle );
			m_free.GetWrite().Add( handle.ID );
		}

		EntityHandle& handle = m_entities.GetWrite()[ m_free.GetWrite()[ 0 ] ];
		m_free.GetWrite().Remove( 0 );

		return handle;
	}

	//
	// Destroy an entity.
	// 
	void EntityManager::Destroy( EntityHandle handle )
	{
		if( handle.ID == EntityHandle::Invalid )
			return;

		if( handle.ID < m_entities.GetWrite().Size() )
		{
			if( m_entities.GetWrite()[ handle.ID ].Version != handle.Version )
				return;

			m_entities.GetWrite()[ handle.ID ].Version++;
			m_free.GetWrite().Add( handle.ID );

			for( auto& it : m_pools )
			{
				ComponentPoolBase* pool = reinterpret_cast<ComponentPoolBase*>(it.Value->GetWrite());
				if( pool->Exists( handle ) )
				{
					pool->Remove( handle );
				}
			}
		}
	}

	void EntityManager::DestroyAll()
	{
		for( EntityHandle entity : m_entities.GetWrite() )
		{
			Destroy( entity );
		}
	}

	bool EntityManager::IsEntityValid( EntityHandle entity )
	{
		if( entity.ID == EntityHandle::Invalid || entity.ID < 0 )
			return false;

		// check readable
		if( entity.ID < m_entities.GetRead().Size() )
		{
			EntityHandle readEntity = m_entities.GetRead()[ entity.ID ];
			if( readEntity.Version == entity.Version )
				return true;
		}

		// check writable
		if( entity.ID < m_entities.GetWrite().Size() )
		{
			EntityHandle writeEntity = m_entities.GetWrite()[ entity.ID ];
			if( writeEntity.Version == entity.Version )
				return true;
		}
		
		return false;
	}
}