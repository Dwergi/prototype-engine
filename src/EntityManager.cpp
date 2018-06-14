//
// EntityManager.cpp - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "EntityManager.h"
#include "IComponentPool.h"

namespace
{
	const unsigned int Maximum = ((unsigned int) -1) - 1;

	const unsigned int DefaultEntityCount = 512;
	const unsigned int DefaultCommandCount = 32;
}

namespace dd
{
	EntityManager::EntityManager() :
		m_free( new std::vector<int>(), new std::vector<int>() ),
		m_entities( new std::vector<EntityHandle>(), new std::vector<EntityHandle>() )
	{
	}

	EntityManager::~EntityManager()
	{
		Update( 0.0f );
		DestroyAll();

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
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		std::vector<int>& free = m_free.GetWrite();
		std::vector<EntityHandle>& entities = m_entities.GetWrite();

		if( free.empty() )
		{
			EntityHandle handle;
			handle.ID = entities.size();
			handle.Version = 0;
			handle.m_manager = this;

			entities.push_back( handle );
			free.push_back( handle.ID );
		}

		EntityHandle& handle = entities[ free.front() ];
		free.erase( free.begin() );

		return handle;
	}

	//
	// Destroy an entity.
	// 
	void EntityManager::Destroy( EntityHandle handle )
	{
		if( handle.ID == EntityHandle::Invalid )
			return;

		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		if( handle.ID < m_entities.GetWrite().size() )
		{
			if( m_entities.GetWrite()[ handle.ID ].Version != handle.Version )
				return;

			m_entities.GetWrite()[ handle.ID ].Version++;
			m_free.GetWrite().push_back( handle.ID );

			for( auto& it : m_pools )
			{
				IComponentPool* pool = reinterpret_cast<IComponentPool*>(it.Value->GetWriteVoid());
				if( pool->Exists( handle ) )
				{
					pool->Remove( handle );
				}
			}
		}
	}

	void EntityManager::DestroyAll()
	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		for( EntityHandle entity : m_entities.GetWrite() )
		{
			Destroy( entity );
		}
	}

	bool EntityManager::IsEntityValid( EntityHandle entity )
	{
		if( entity.ID == EntityHandle::Invalid || entity.ID < 0 )
			return false;

		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		// check readable
		if( entity.ID < m_entities.GetRead().size() )
		{
			EntityHandle readEntity = m_entities.GetRead()[ entity.ID ];
			if( readEntity.Version == entity.Version )
				return true;
		}

		// check writable
		if( entity.ID < m_entities.GetWrite().size() )
		{
			EntityHandle writeEntity = m_entities.GetWrite()[ entity.ID ];
			if( writeEntity.Version == entity.Version )
				return true;
		}
		
		return false;
	}
}