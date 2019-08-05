#include "PCH.h"
#include "EntitySpace.h"

namespace ddc
{
	static_assert( sizeof( Entity ) <= sizeof( uint ) + sizeof( uint ) + sizeof( EntitySpace* ) );

	EntitySpace::EntitySpace(std::string name) :
		m_name(name)
	{
		m_components.resize( dd::TypeInfo::ComponentCount() );

		for( dd::ComponentID i = 0; i < m_components.size(); ++i )
		{
			const dd::TypeInfo* type = dd::TypeInfo::GetComponent( i );
			DD_ASSERT( type != nullptr );

			size_t buffer_size = type->Size() * MAX_ENTITIES;
			DD_ASSERT( buffer_size < 1024 * 1024 * 1024 );

			m_components[i] = new byte[buffer_size];
			memset( m_components[i], 0, buffer_size );
		}
	}

	void EntitySpace::Update( float delta_t )
	{
		for( int id = 0; id < m_entities.size(); ++id )
		{
			EntityEntry& entry = m_entities[id];
			if( entry.Destroy )
			{
				entry.Alive = false;
				entry.Destroy = false;
				entry.Create = false;

				entry.Ownership.reset();
				entry.Tags.reset();

				m_free.push_back(id);
			}

			if( entry.Create )
			{
				entry.Create = false;
				entry.Alive = true;
			}
		}
	}

	Entity EntitySpace::CreateEntity()
	{
		if( m_free.empty() )
		{
			m_free.push_back( (uint) m_entities.size() );

			EntityEntry new_entry;
			new_entry.Entity.ID = m_entities.size();
			new_entry.Entity.Version = -1;
			new_entry.Entity.m_space = this;

			m_entities.push_back( new_entry );
		}

		uint idx = dd::pop_front( m_free );

		if( idx > MAX_ENTITIES )
		{
			DD_ASSERT( false, "Went over the max entity count!" );
			throw std::exception( "Went over the max entity count!" );
		}

		EntityEntry& entry = m_entities[ idx ];
		entry.Entity.Version++;
		entry.Create = true;
		return entry.Entity;
	}

	void EntitySpace::DestroyEntity( Entity entity )
	{
		DD_ASSERT( IsAlive( entity ), "Entity being destroyed is not alive, ID: %d, Version: %d", entity.ID, entity.Version );

		m_entities[ entity.ID ].Destroy = true;
	}

	Entity EntitySpace::GetEntity( uint id ) const
	{
		if( id < m_entities.size() )
		{
			return m_entities[ id ].Entity;
		}

		return Entity();
	}

	bool EntitySpace::IsAlive( Entity entity ) const
	{
		DD_ASSERT( entity.ID >= 0 && entity.ID < m_entities.size() );

		const EntityEntry& entry = m_entities[ entity.ID ];

		return entry.Entity.Version == entity.Version &&
			(entry.Alive || entry.Create);
	}

	bool EntitySpace::HasComponent( Entity entity, dd::ComponentID id ) const
	{
		DD_ASSERT( id != dd::INVALID_COMPONENT );

		if( !IsAlive( entity ) )
		{
			return false;
		}

		return m_entities[entity.ID].Ownership.test( id );
	}

	void* EntitySpace::AddComponent( Entity entity, dd::ComponentID id )
	{
		if( HasComponent( entity, id ) )
		{
			return AccessComponent( entity, id );
		}

		m_entities[entity.ID].Ownership.set( id, true );

		void* ptr = AccessComponent( entity, id );
		DD_ASSERT( ptr != nullptr );

		const dd::TypeInfo* type = dd::TypeInfo::GetComponent( id ); 
		type->PlacementNew( ptr );
		return ptr;
	}

	void* EntitySpace::AccessComponent( Entity entity, dd::ComponentID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		const dd::TypeInfo* type = dd::TypeInfo::GetComponent( id );
		return m_components[id] + (entity.ID * type->Size());
	}

	const void* EntitySpace::GetComponent( Entity entity, dd::ComponentID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}
		
		const dd::TypeInfo* type = dd::TypeInfo::GetComponent( id );
		return m_components[id] + (entity.ID * type->Size());
	}

	void EntitySpace::RemoveComponent( Entity entity, dd::ComponentID id )
	{
		if( HasComponent( entity, id ) )
		{
			m_entities[entity.ID].Ownership.set( id, false );
		}
		else
		{
			DD_ASSERT( false, "Entity does not have have component being removed!" );
		}
	}

	void EntitySpace::GetAllComponents( Entity entity, dd::IArray<dd::ComponentID>& components ) const
	{
		DD_ASSERT( IsAlive( entity ) );

		for( dd::ComponentID i = 0; i < MAX_COMPONENTS; ++i )
		{
			if( m_entities[ entity.ID ].Ownership.test( i ) )
			{
				components.Add( i );
			}
		}
	}

	void EntitySpace::FindAllWith( const dd::IArray<dd::ComponentID>& components, const TagBits& tags, std::vector<Entity>& outEntities ) const
	{
		ComponentBits required;
		for( dd::ComponentID type : components )
		{
			required.set( type, true );
		}

		for( const EntityEntry& entry : m_entities )
		{
			if( IsAlive( entry.Entity ) )
			{
				TagBits entity_tags = tags & entry.Tags;
				ComponentBits entity_components = required & entry.Ownership;

				if( entity_components.count() == required.count() && 
					entity_tags.count() == tags.count() )
				{
					outEntities.push_back( entry.Entity );
				}
			}
		}
	}

	bool EntitySpace::HasTag( Entity e, Tag tag ) const
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		return m_entities[ e.ID ].Tags.test( (uint) tag );
	}

	void EntitySpace::AddTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.set( (uint) tag );
	}

	void EntitySpace::RemoveTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.reset( (uint) tag );
	}

	void EntitySpace::SetAllTags( Entity e, TagBits tags )
	{
		DD_ASSERT( IsAlive( e ) );

		m_entities[ e.ID ].Tags = tags;
	}

	TagBits EntitySpace::GetAllTags( Entity e ) const
	{
		DD_ASSERT( IsAlive( e ) );

		return m_entities[ e.ID ].Tags;
	}
}