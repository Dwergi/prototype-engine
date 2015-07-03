#pragma once

#include "EntityHandle.h"

namespace dd
{
	template< typename ComponentType >
	class ComponentHandle
	{
	private:
		typedef typename ComponentType::Pool PoolType;

	public:

		ComponentHandle( dd::EntityHandle entity, const PoolType& pool )
			: m_pool( pool ),
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
			return m_entity.IsValid() && m_pool.Exists( m_entity );
		}

		ComponentType* Get() const 
		{
			if( !m_entity.IsValid() )
				return nullptr;

			return m_pool.Find( m_entity )
		}

		ComponentType* operator->()
		{
			ASSERT( IsValid(), "Handle not valid!" );

			return m_pool->Find( m_entity );
		}

		ComponentType& operator*()
		{
			ASSERT( IsValid(), "Handle not valid!" );

			return *m_pool->Find( m_entity );
		}

		ComponentType* operator->() const
		{
			ASSERT( IsValid(), "Handle not valid!" );

			return m_pool->Find( m_entity );
		}

		ComponentType& operator*() const
		{
			ASSERT( IsValid(), "Handle not valid!" );

			return *m_pool->Find( m_entity );
		}

	private:

		const PoolType& m_pool;
		dd::EntityHandle m_entity;
	};
}