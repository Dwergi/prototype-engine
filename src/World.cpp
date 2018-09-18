#include "PrecompiledHeader.h"
#include "World.h"

#include "JobSystem.h"
#include "UpdateData.h"
#include "System.h"
#include "SystemsSorting.h"

namespace ddc
{
	ComponentType* ComponentType::Types[MAX_COMPONENTS] = { nullptr };
	int ComponentType::Count = 0;

	DD_STATIC_ASSERT( sizeof( Entity ) <= sizeof( uint ) + sizeof( uint ) );

	static void WaitForAllFutures( std::vector<std::shared_future<void>>& futures )
	{
		size_t ready = 0;
		while( ready < futures.size() )
		{
			ready = 0;
			for( std::shared_future<void>& f : futures )
			{
				std::future_status s = f.wait_for( std::chrono::microseconds( 1 ) );
				if( s == std::future_status::ready )
				{
					++ready;
				}
			}
		}
	}

	World::World( dd::JobSystem& jobsystem ) :
		m_jobsystem( jobsystem )
	{
		m_components.resize( ComponentType::Count );
		for( int i = 0; i < ComponentType::Count; ++i )
		{
			const ComponentType* type = ComponentType::Types[i];
			size_t buffer_size = type->Size * MAX_ENTITIES;

			m_components[i] = new byte[buffer_size];
			memset( m_components[i], 0, buffer_size );
		}
	}

	void World::Initialize()
	{
		for( System* system : m_systems )
		{
			system->Initialize( *this );
		}

		ddc::OrderSystemsByDependencies( dd::Span<System*>( m_systems ), m_orderedSystems );
	}

	void World::Shutdown()
	{
		for( System* system : m_systems )
		{
			system->Shutdown( *this );
		}
	}

	void World::Update( float delta_t )
	{
		for( EntityEntry& entry : m_entities )
		{
			if( entry.Entity.Destroy )
			{
				entry.Entity.Alive = false;
				entry.Entity.Destroy = false;
				entry.Entity.Create = false;

				entry.Ownership.reset();
				entry.Tags.reset();
			}

			if( entry.Entity.Create )
			{
				entry.Entity.Alive = true;
			}
		}

		UpdateSystemsWithTreeScheduling( delta_t );

		std::vector<std::shared_future<void>> updates;
		updates.reserve( m_orderedSystems.size() );

		for( SystemNode& s : m_orderedSystems )
		{
			updates.push_back( s.m_update );
		}

		WaitForAllFutures( updates );

		for( SystemNode& s : m_orderedSystems )
		{
			s.m_update = std::shared_future<void>();
		}
	}

	void World::RegisterSystem( System& system )
	{
		m_systems.push_back( &system );
	}

	Entity World::CreateEntity()
	{
		if( m_free.empty() )
		{
			m_free.push_back( m_count );

			EntityEntry new_entry;
			new_entry.Entity.ID = m_count;
			new_entry.Entity.Version = -1;

			m_entities.push_back( new_entry );

			++m_count;
		}

		int idx = dd::pop_front( m_free );

		EntityEntry& entry = m_entities[ idx ];
		entry.Entity.Version++;
		entry.Entity.Create = true;
		return entry.Entity;
	}

	void World::DestroyEntity( Entity entity )
	{
		DD_ASSERT( IsAlive( entity ) );

		m_free.push_back( entity.ID );
		m_entities[ entity.ID ].Entity.Destroy = true;
	}

	Entity World::GetEntity( uint id ) const
	{
		if( id < m_entities.size() )
		{
			return m_entities[ id ].Entity;
		}

		return Entity();
	}

	bool World::IsAlive( Entity entity ) const
	{
		DD_ASSERT( entity.ID >= 0 && entity.ID < m_entities.size() );

		const EntityEntry& entry = m_entities[ entity.ID ];

		return entry.Entity.Version == entity.Version &&
			(entry.Entity.Alive || entry.Entity.Create);
	}

	bool World::HasComponent( Entity entity, TypeID id ) const
	{
		if( !IsAlive( entity ) )
		{
			return false;
		}

		return m_entities[entity.ID].Ownership.test( id );
	}

	void* World::AddComponent( Entity entity, TypeID id )
	{
		if( HasComponent( entity, id ) )
		{
			return AccessComponent( entity, id );
		}

		m_entities[entity.ID].Ownership.set( id, true );

		void* ptr = AccessComponent( entity, id );
		DD_ASSERT( ptr != nullptr );

		ComponentType::Types[id]->Construct( ptr );
		return ptr;
	}

	void* World::AccessComponent( Entity entity, TypeID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		return m_components[id] + (entity.ID * ComponentType::Types[id]->Size);
	}

	const void* World::GetComponent( Entity entity, TypeID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		return m_components[id] + (entity.ID * ComponentType::Types[id]->Size);
	}

	void World::RemoveComponent( Entity entity, TypeID id )
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

	void World::FindAllWith( const dd::IArray<TypeID>& components, const std::bitset<MAX_TAGS>& tags, std::vector<Entity>& outEntities ) const
	{
		std::bitset<MAX_COMPONENTS> required;
		for( TypeID& type : components )
		{
			required.set( type, true );
		}

		for( int i = 0; i < m_count; ++i )
		{
			const EntityEntry& entry = m_entities[ i ];

			if( IsAlive( entry.Entity ) )
			{
				std::bitset<MAX_TAGS> entity_tags = tags & entry.Tags;
				std::bitset<MAX_COMPONENTS> entity_components = required & m_entities[i].Ownership;

				if( entity_components.count() == required.count() && 
					entity_tags.count() == tags.count() )
				{
					outEntities.push_back( m_entities[i].Entity );
				}
			}
		}
	}

	bool World::HasTag( Entity e, Tag tag ) const
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		return m_entities[ e.ID ].Tags.test( (uint) tag );
	}

	void World::AddTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.set( (uint) tag );
	}

	void World::RemoveTag( Entity e, Tag tag )
	{
		DD_ASSERT( IsAlive( e ) );
		DD_ASSERT( tag != Tag::None );

		m_entities[ e.ID ].Tags.reset( (uint) tag );
	}

	void World::UpdateSystem( System* system, std::vector<std::shared_future<void>> dependencies, float delta_t )
	{
		WaitForAllFutures( dependencies );

		// filter entities that have the requirements
		dd::Array<TypeID, MAX_COMPONENTS> required;
		for( const DataRequest* req : system->GetRequests() )
		{
			if( !req->Optional() )
			{
				required.Add( req->Component().ID );
			}
		}

		const std::bitset<MAX_TAGS>& tags = system->GetRequiredTags();

		std::vector<Entity> entities;
		FindAllWith( required, tags, entities );

		dd::Span<Entity> entity_span( entities );

		UpdateData data( *this, entity_span, system->GetRequests(), delta_t );
		system->Update( data );

		data.Commit();
	}

	std::vector<std::shared_future<void>> GetFutures( SystemNode& s, std::vector<SystemNode>& systems )
	{
		std::vector<std::shared_future<void>> futures;

		for( SystemNode::Edge& e : s.m_in )
		{
			SystemNode& dep = systems[ e.m_from ];
			futures.push_back( dep.m_update );
		}

		return futures;
	}

	void World::UpdateSystemsWithTreeScheduling( float delta_t )
	{
		for( SystemNode& s : m_orderedSystems )
		{
			DD_ASSERT( s.m_system != nullptr );

			s.m_update = m_jobsystem.Schedule( [this, &s, delta_t]()
			{
				UpdateSystem( s.m_system, GetFutures( s, m_orderedSystems ), delta_t );
			} ).share();
		}
	}
}