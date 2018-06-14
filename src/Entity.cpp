#include "PrecompiledHeader.h"
#include "Entity.h"

namespace ddc
{
	ComponentType* ComponentType::Types[ MAX_COMPONENTS ] = { nullptr };
	int ComponentType::Count = 0;

	EntitySpace::EntitySpace()
	{
		m_alive.reset();

		m_components.resize( ComponentType::Count );
		for( int i = 0; i < ComponentType::Count; ++i )
		{
			const ComponentType* type = ComponentType::Types[ i ];
			m_components[ i ] = new byte[ type->Size * MAX_ENTITIES ];
			memset( m_components[ i ], 0, type->Size * MAX_ENTITIES );
		}
	}
	
	Entity EntitySpace::Create()
	{
		if( m_free.empty() )
		{
			m_free.push_back( m_count );
			m_ownership.push_back( 0 );
			++m_count;
		}

		Entity entity = m_free.front();
		
		m_free.erase( m_free.begin() );
		m_alive.set( entity, true );

		return entity;
	}

	void EntitySpace::Destroy( Entity entity )
	{
		DD_ASSERT( m_alive.test( entity ) );

		m_free.push_back( entity );
		m_alive.set( entity, false );
	}

	bool EntitySpace::IsAlive( Entity entity )
	{
		return m_alive.test( entity );
	}

	void* EntitySpace::AddComponent( Entity entity, TypeID id )
	{
		if( !HasComponent( entity, id ) )
		{
			m_ownership[ entity ].set( id, true );
		}

		return AccessComponent( entity, id );
	}

	void* EntitySpace::AccessComponent( Entity entity, TypeID id )
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		return static_cast<byte*>(m_components[ id ]) + (entity * ComponentType::Types[ id ]->Size);
	}

	void EntitySpace::RemoveComponent( Entity entity, TypeID id )
	{
		if( HasComponent( entity, id ) )
		{
			m_ownership[ entity ].set( id, false );
		}
	}

	void EntitySpace::FindAllWith( const dd::IArray<int>& components, std::vector<int>& outEntities )
	{
		std::bitset<MAX_COMPONENTS> mask;
		for( int id : components )
		{
			mask.set( id, true );
		}

		for( int i = 0; i < m_count; ++i )
		{
			std::bitset<MAX_COMPONENTS> entity_mask = mask;
			mask &= m_ownership[ i ];

			if( IsAlive( i ) &&	mask.any() )
			{
				outEntities.push_back( i );
			}
		}
	}
}