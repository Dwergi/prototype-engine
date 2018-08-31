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

	World::World()
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

		for( System* system : m_systems )
		{
			UpdateSystem( system, delta_t );
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

	void World::UpdateSystem( System* system, float delta_t )
	{
		// filter entities that have the requirements
		dd::Array<TypeID, MAX_COMPONENTS> components;
		for( const DataRequirement* req : system->GetRequirements() )
		{
			components.Add( req->Component().ID );
		}

		std::vector<Entity> entities;
		FindAllWith( components, entities );

		int partition_count = system->MaxPartitions();

		size_t partition_size = entities.size() / partition_count;

		size_t entity_start = 0;
		for( int partition = 0; partition < partition_count; ++partition )
		{
			size_t entity_count = partition_size;

			if( partition == 0 )
			{
				size_t remainder = entities.size() - partition_size * partition_count;
				entity_count = partition_size + remainder;
			}

			dd::Span<Entity> entity_span( entities, entity_count, entity_start );

			UpdateData data( *this, entity_span, system->GetRequirements() );

			system->Update( data, delta_t );

			data.Commit();

			entity_start += entity_count;
		}
	}

	static void WaitForAllDependencies( SystemNode& s, std::vector<SystemNode>& systems )
	{
		size_t ready = 0;

		while( ready < s.m_in.size() )
		{
			for( SystemNode::Edge& e : s.m_in )
			{
				SystemNode& dep = systems[ e.m_from ];
				std::future_status s = dep.m_update.wait_for( std::chrono::microseconds( 1 ) );

				if( s == std::future_status::ready )
				{
					++ready;
				}
			}
		}
	}

	void World::UpdateSystemsWithTreeScheduling( std::vector<SystemNode>& systems, dd::JobSystem& jobsystem, float delta_t )
	{
		for( SystemNode& s : systems )
		{
			if( s.m_system == nullptr )
				continue;

			std::future<void> f = jobsystem.Schedule( [this, &s, &systems, delta_t]()
			{
				WaitForAllDependencies( s, systems );
				UpdateSystem( s.m_system, delta_t );
			} );

			s.m_update = f.share();
		}
	}
}