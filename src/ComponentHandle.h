//
// ComponentHandle.h - A handle to a component.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "EntityHandle.h"

namespace dd
{
	template<typename T>
	class ComponentPool;

	//
	// A component handle is meant to be stored for a long time. 
	// It's not the most efficient way to access a component repeatedly in a short span,
	// but it is more tolerant to components ceasing to exist.
	//
	template< typename ComponentType >
	class ComponentHandle
	{
	public:

		ComponentHandle( EntityHandle entity, const ComponentPool<ComponentType>& pool )
			: m_pool( &pool ),
			m_entity( entity )
		{
		}

		ComponentHandle( const ComponentHandle& other )
			: m_pool( other.m_pool ),
			m_entity( other.m_entity )
		{
		}

		bool IsValid() const
		{
			return m_pool != nullptr && m_entity.IsValid() && m_pool->Exists( m_entity );
		}

		ComponentType* Get() const 
		{
			DD_ASSERT( m_pool != nullptr );
			DD_ASSERT( IsValid(), "Handle not valid!" );

			if( !IsValid() )
				return nullptr;

			return m_pool->Find( m_entity )
		}

		ComponentType* operator->()
		{
			return Get();
		}

		ComponentType& operator*()
		{
			DD_ASSERT( IsValid(), "Handle not valid!" );

			return *m_pool->Find( m_entity );
		}

		ComponentType* operator->() const
		{
			DD_ASSERT( IsValid(), "Handle not valid!" );

			return m_pool->Find( m_entity );
		}

		ComponentType& operator*() const
		{
			DD_ASSERT( IsValid(), "Handle not valid!" );

			return *m_pool->Find( m_entity );
		}

	private:

		const ComponentPool<ComponentType>* m_pool;
		EntityHandle m_entity;
	};
}