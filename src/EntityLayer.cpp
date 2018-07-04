#include "PrecompiledHeader.h"
#include "EntityLayer.h"

#include "JobSystem.h"
#include "UpdateData.h"
#include "System.h"

namespace ddc
{
	ComponentType* ComponentType::Types[ MAX_COMPONENTS ] = { nullptr };
	int ComponentType::Count = 0;

	DD_STATIC_ASSERT( sizeof( Entity ) == sizeof( int ) );

	EntityLayer::EntityLayer()
	{
		m_components.resize( ComponentType::Count );
		for( int i = 0; i < ComponentType::Count; ++i )
		{
			const ComponentType* type = ComponentType::Types[ i ];
			m_components[ i ] = new byte[ type->Size * MAX_ENTITIES ];
			memset( m_components[ i ], 0, type->Size * MAX_ENTITIES );
		}
	}
	
	Entity EntityLayer::Create()
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
		entity.Alive = true;
		return entity;
	}

	void EntityLayer::Destroy( Entity entity )
	{
		DD_ASSERT( IsAlive( entity ) );

		m_free.push_back( entity.ID );
		m_entities[ entity.ID ].Alive = false;
	}

	bool EntityLayer::IsAlive( Entity entity ) const
	{
		DD_ASSERT( entity.ID >= 0 && entity.ID < m_entities.size() );
		return m_entities[ entity.ID ].Version == entity.Version && m_entities[ entity.ID ].Alive;
	}

	bool EntityLayer::HasComponent( Entity entity, TypeID id ) const
	{
		if( !IsAlive( entity ) )
		{
			return false;
		}

		return m_ownership[ entity.ID ].test( id );
	}

	void* EntityLayer::AddComponent( Entity entity, TypeID id )
	{
		if( !HasComponent( entity, id ) )
		{
			m_ownership[ entity.ID ].set( id, true );
		}

		void* ptr = AccessComponent( entity, id );
		ComponentType::Types[ id ]->Construct( ptr );
		return ptr;
	}

	void* EntityLayer::AccessComponent( Entity entity, TypeID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		return m_components[ id ] + (entity.ID * ComponentType::Types[ id ]->Size);
	}

	const void* EntityLayer::GetComponent( Entity entity, TypeID id ) const
	{
		if( !HasComponent( entity, id ) )
		{
			return nullptr;
		}

		return m_components[ id ] + (entity.ID * ComponentType::Types[ id ]->Size);
	}

	void EntityLayer::RemoveComponent( Entity entity, TypeID id )
	{
		if( HasComponent( entity, id ) )
		{
			m_ownership[ entity.ID ].set( id, false );
		}
		else
		{
			DD_ASSERT( false, "Entity does not have have component being removed!" );
		}
	}

	void EntityLayer::FindAllWith( const dd::IArray<int>& components, std::vector<Entity>& outEntities ) const 
	{
		std::bitset<MAX_COMPONENTS> mask;
		for( int id : components )
		{
			mask.set( id, true );
		}

		for( int i = 0; i < m_count; ++i )
		{
			std::bitset<MAX_COMPONENTS> entity_mask = mask;
			entity_mask &= m_ownership[ i ];

			if( m_entities[ i ].Alive && entity_mask.any() )
			{
				outEntities.push_back( m_entities[ i ] );
			}
		}
	}

	void UpdateSystem( System& system, EntityLayer& layer, int partition_count )
	{
		// filter entities that have the requirements
		dd::Array<TypeID, MAX_COMPONENTS> components;
		for( const DataRequirement* req : system.GetRequirements() )
		{
			components.Add( req->Component().ID );
		}

		dd::Span<TypeID> cmp_span( components.Data(), components.Size() );

		std::vector<Entity> entities;
		layer.FindAllWith( components, entities );

		if( entities.size() == 0 )
		{
			return;
		}

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

			if( entity_count == 0 )
			{
				continue;
			}

			dd::Span<Entity> entity_span( entities, entity_count, entity_start );

			UpdateData data( layer, entity_span, system.GetRequirements() );

			system.Update( data );

			data.Commit();

			entity_start += entity_count;
		}
	}

	static void SortSystemsTopologically( const std::vector<SystemNode>& nodes, std::vector<SystemNode>& out_sorted )
	{
		std::vector<SystemNode> scratch = nodes;

		out_sorted.clear();

		while( out_sorted.size() < nodes.size() )
		{
			for( size_t n = 0; n < scratch.size(); ++n )
			{
				SystemNode& node = scratch[ n ];

				if( node.m_system != nullptr && node.m_in.size() == 0 )
				{
					out_sorted.push_back( nodes[ n ] );

					for( SystemNode::Edge& outEdge : node.m_out )
					{
						SystemNode& destNode = scratch[ outEdge.m_to ];

						size_t i = 0;
						for( ; i < destNode.m_in.size(); ++i )
						{
							SystemNode::Edge& inEdge = destNode.m_in[ i ];
							if( inEdge.m_from == n )
							{
								break;
							}
						}

						destNode.m_in.erase( destNode.m_in.begin() + i );
					}

					node.m_out.clear();
					node.m_system = nullptr;
				}
			}
		}
	}

	void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems )
	{
		std::vector<SystemNode> nodes;
		nodes.reserve( systems.Size() );

		for( System* system : systems )
		{
			SystemNode node;
			node.m_system = system;
			nodes.push_back( node );
		}

		for( int sys = 0; sys < systems.Size(); ++sys )
		{
			System* system = systems[ sys ];

			for( const DataRequirement* req : system->GetRequirements() )
			{
				if( req->Usage() == DataUsage::Write )
				{
					for( int other = 0; other < systems.Size(); ++other )
					{
						if( sys == other )
							continue;

						System* other_system = systems[ other ];

						for( const DataRequirement* other_req : other_system->GetRequirements() )
						{
							if( other_req->Component().ID == req->Component().ID &&
								other_req->Usage() == DataUsage::Read )
							{
								SystemNode::Edge edge;
								edge.m_from = sys;
								edge.m_to = other;

								nodes[ sys ].m_out.push_back( edge );
								nodes[ other ].m_in.push_back( edge );
							}
						}
					}
				}
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems )
	{
		std::vector<SystemNode> nodes;
		nodes.reserve( systems.Size() );

		for( System* system : systems )
		{
			SystemNode node;
			node.m_system = system;
			nodes.push_back( node );
		}

		for( size_t sys = 0; sys < systems.Size(); ++sys )
		{
			const System* system = systems[ sys ];

			const dd::IArray<const System*>& deps = system->GetDependencies();
			for( size_t dep = 0; dep < deps.Size(); ++dep )
			{
				DD_ASSERT( system != systems[ dep ], "Can't depend on the same system!" );

				SystemNode::Edge edge;
				edge.m_from = dep;
				edge.m_to = sys;
				
				nodes[ dep ].m_out.push_back( edge );
				nodes[ sys ].m_in.push_back( edge );
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}

	void WaitForAllDependencies( SystemNode& s, std::vector<SystemNode>& systems )
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

	void UpdateSystemsWithTreeScheduling( std::vector<SystemNode>& systems, dd::JobSystem& jobsystem, EntityLayer& layer )
	{
		for( SystemNode& s : systems )
		{
			if( s.m_system == nullptr )
				continue;

			std::future<void> f = jobsystem.Schedule( [&layer, &s, &systems]()
			{
				WaitForAllDependencies( s, systems );
				UpdateSystem( *(s.m_system), layer, 1 );
			} );

			s.m_update = f.share();
		}
	}
}