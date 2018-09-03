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
		for( Entity& entity : m_entities )
		{
			if( entity.Destroy )
			{
				entity.Alive = false;
				entity.Destroy = false;
				entity.Create = false;
			}

			if( entity.Create )
			{
				entity.Alive = true;
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

			Entity new_entity;
			new_entity.ID = m_count;
			new_entity.Version = -1;

			m_entities.push_back( new_entity );
			m_ownership.push_back( 0 );

			++m_count;
		}

		int idx = dd::pop_front( m_free );

		Entity& entity = m_entities[ idx ];
		entity.Version++;
		entity.Create = true;
		return entity;
	}

	void World::DestroyEntity( Entity entity )
	{
		DD_ASSERT( IsAlive( entity ) );

		m_free.push_back( entity.ID );
		m_entities[ entity.ID ].Destroy = true;
	}

	Entity World::GetEntity( uint id ) const
	{
		if( id < m_entities.size() )
		{
			return m_entities[ id ];
		}

		return Entity();
	}

	bool World::IsAlive( Entity entity ) const
	{
		DD_ASSERT( entity.ID >= 0 && entity.ID < m_entities.size() );

		return m_entities[ entity.ID ].Version == entity.Version &&
			(m_entities[ entity.ID ].Alive || m_entities[ entity.ID ].Create);
	}

	bool World::HasComponent( Entity entity, TypeID id ) const
	{
		if( !IsAlive( entity ) )
		{
			return false;
		}

		return m_ownership[entity.ID].test( id );
	}

	void* World::AddComponent( Entity entity, TypeID id )
	{
		if( HasComponent( entity, id ) )
		{
			return AccessComponent( entity, id );
		}

		m_ownership[entity.ID].set( id, true );

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
		if( entity.ID > 10000 )
		{
			__debugbreak();
		}

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
			m_ownership[entity.ID].set( id, false );
		}
		else
		{
			DD_ASSERT( false, "Entity does not have have component being removed!" );
		}
	}

	void World::FindAllWith( const dd::IArray<TypeID>& components, std::vector<Entity>& outEntities ) const
	{
		std::bitset<MAX_COMPONENTS> required;
		for( TypeID& type : components )
		{
			required.set( type, true );
		}

		for( int i = 0; i < m_count; ++i )
		{
			if( m_entities[ i ].Alive )
			{
				std::bitset<MAX_COMPONENTS> entity_mask = required & m_ownership[i];
				if( entity_mask.count() == required.count() )
				{
					outEntities.push_back( m_entities[ i ] );
				}
			}
		}
	}

	void World::UpdateSystem( System* system, std::vector<std::shared_future<void>> dependencies, float delta_t )
	{
		WaitForAllFutures( dependencies );

		// filter entities that have the requirements
		dd::Array<TypeID, MAX_COMPONENTS> components;
		for( const DataRequirement* req : system->GetRequirements() )
		{
			components.Add( req->Component().ID );
		}

		std::vector<Entity> entities;
		FindAllWith( components, entities );

		dd::Span<Entity> entity_span( entities );

		UpdateData data( *this, entity_span, system->GetRequirements() );
		system->Update( data, delta_t );

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