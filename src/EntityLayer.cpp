//
// EntityLayer.cpp
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#include "PCH.h"
#include "EntityLayer.h"

namespace ddc
{
	static_assert( sizeof( ddc::Entity ) == sizeof( uint64 ), "Entity should only be 64 bits." );

	static const uint8 MAX_SPACES = 8;

	static EntityLayer* s_spaceInstances[MAX_SPACES];
	static uint8 s_maxLayer = 0;

	EntityLayer::EntityLayer(std::string name) :
		m_name(name)
	{
		DD_ASSERT(s_maxLayer < MAX_SPACES);
		s_spaceInstances[s_maxLayer] = this;
		m_instanceIndex = s_maxLayer;
		++s_maxLayer;

		m_maxEntities = 1024;
		UpdateStorage();
	}

	EntityLayer::~EntityLayer()
	{
		for (byte* buffer : m_components)
		{
			free(buffer);
		}
	}

	void EntityLayer::UpdateStorage()
	{
		if (m_entities.size() <= m_maxEntities && m_components.size() == dd::TypeInfo::ComponentCount())
		{
			return;
		}
		
		uint old_max_entities = m_maxEntities;
		if (m_entities.size() > m_maxEntities)
		{
			m_maxEntities *= 2;
		}

		m_components.resize(dd::TypeInfo::ComponentCount());

		for (dd::ComponentID i = 0; i < dd::TypeInfo::ComponentCount(); ++i)
		{
			const dd::TypeInfo* type = dd::TypeInfo::GetComponent(i);
			DD_ASSERT(type != nullptr);

			size_t buffer_size = type->Size() * m_maxEntities;
			DD_ASSERT(buffer_size < 1024 * 1024 * 1024);

			if (m_components[i] == nullptr)
			{
				m_components[i] = (byte*) malloc(buffer_size);
				memset(m_components[i], 0, buffer_size);
			}
			else
			{
				size_t old_buffer_size = old_max_entities * type->Size();
				m_components[i] = (byte*) realloc(m_components[i], buffer_size);
				memset(m_components[i] + old_buffer_size, 0, buffer_size - old_buffer_size);
			}
		}
	}

	void EntityLayer::Update( float delta_t )
	{
		UpdateStorage();

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

	Entity EntityLayer::CreateEntity()
	{
		if( m_free.empty() )
		{
			m_free.push_back( (uint) m_entities.size() );

			EntityEntry new_entry;
			new_entry.Entity.ID = m_entities.size();
			new_entry.Entity.Version = -1;
			new_entry.Entity.m_layer = m_instanceIndex;

			m_entities.push_back( new_entry );
			UpdateStorage();
		}

		uint idx = dd::pop_front( m_free );
		DD_ASSERT(idx < m_maxEntities, "Went over the max entity count!");

		EntityEntry& entry = m_entities[ idx ];
		entry.Entity.Version++;
		entry.Create = true;
		return entry.Entity;
	}

	void EntityLayer::DestroyEntity( Entity entity )
	{
		DD_ASSERT( IsAlive( entity ), "Entity being destroyed is not alive, ID: %d, Version: %d", entity.ID, entity.Version );

		m_entities[ entity.ID ].Destroy = true;
	}

	Entity EntityLayer::GetEntity( uint id ) const
	{
		if( id < m_entities.size() )
		{
			return m_entities[ id ].Entity;
		}

		return Entity();
	}

	bool EntityLayer::IsAlive( Entity entity ) const
	{
		DD_ASSERT( entity.ID >= 0 && entity.ID < m_entities.size() );

		const EntityEntry& entry = m_entities[ entity.ID ];

		return entry.Entity.Version == entity.Version && (entry.Alive || entry.Create);
	}

	int EntityLayer::ComponentCount(Entity entity) const
	{
		DD_ASSERT(IsAlive(entity));

		const EntityEntry& entry = m_entities[entity.ID];
		return (int) entry.Ownership.count();
	}

	dd::ComponentID EntityLayer::GetNthComponentID(Entity entity, int index) const
	{
		DD_ASSERT(IsAlive(entity));

		const EntityEntry& entry = m_entities[entity.ID];

		size_t i = 0;
		while (index > 0 && i < MAX_COMPONENTS)
		{
			if (entry.Ownership.test(i))
			{
				--index;
			}
		}

		return (dd::ComponentID) i;
	}

	bool EntityLayer::HasComponent( Entity entity, dd::ComponentID id ) const
	{
		DD_ASSERT(entity.IsValid());

		if( !IsAlive( entity ) )
		{
			return false;
		}

		return m_entities[entity.ID].Ownership.test( id );
	}

	void* EntityLayer::AddComponent( Entity entity, dd::ComponentID id )
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

	void* EntityLayer::AccessComponent( Entity entity, dd::ComponentID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		const dd::TypeInfo* type = dd::TypeInfo::GetComponent( id );
		return m_components[id] + (entity.ID * type->Size());
	}

	const void* EntityLayer::GetComponent( Entity entity, dd::ComponentID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}
		
		const dd::TypeInfo* type = dd::TypeInfo::GetComponent( id );
		return m_components[id] + (entity.ID * type->Size());
	}

	void EntityLayer::RemoveComponent( Entity entity, dd::ComponentID id )
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

	void EntityLayer::GetAllComponents( Entity entity, dd::IArray<dd::ComponentID>& components ) const
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

	void EntityLayer::FindAllWith( const dd::IArray<dd::ComponentID>& components, const TagBits& tags, std::vector<Entity>& outEntities ) const
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

	bool EntityLayer::HasTag( Entity e, Tag tag ) const
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		return m_entities[ e.ID ].Tags.test( (uint) tag );
	}

	void EntityLayer::AddTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.set( (uint) tag );
	}

	void EntityLayer::RemoveTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.reset( (uint) tag );
	}

	void EntityLayer::SetAllTags( Entity e, TagBits tags )
	{
		DD_ASSERT( IsAlive( e ) );

		m_entities[ e.ID ].Tags = tags;
	}

	TagBits EntityLayer::GetAllTags( Entity e ) const
	{
		DD_ASSERT( IsAlive( e ) );

		return m_entities[ e.ID ].Tags;
	}

	EntityLayer* Entity::Layer() const
	{
		if (m_layer < s_maxLayer)
		{
			return s_spaceInstances[m_layer];
		}
		return nullptr;
	}

	bool Entity::IsValid() const
	{
		return Handle != INVALID_HANDLE;
	}

	bool Entity::IsAlive() const
	{
		return IsValid() && Layer()->IsAlive(*this);
	}

	void Entity::AddTag(ddc::Tag tag) const
	{
		Layer()->AddTag(*this, tag);
	}

	void Entity::RemoveTag(ddc::Tag tag) const
	{
		Layer()->RemoveTag(*this, tag);
	}

	bool Entity::HasTag(ddc::Tag tag) const
	{
		return Layer()->HasTag(*this, tag);
	}

	int Entity::Components() const
	{
		return Layer()->ComponentCount(*this);
	}

	void Entity::Destroy() const
	{
		return Layer()->DestroyEntity(*this);
	}
}