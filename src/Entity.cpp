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

	void UpdateSystem( EntitySpace& space, System& system )
	{
		dd::Array<TypeID, MAX_COMPONENTS> nodes;
		for( const DataRequirement* read : system.GetRequirements() )
		{
			nodes.Add( read->Component() );
		}

		dd::Span<TypeID> cmp_span( nodes.Data(), nodes.Size() );

		std::vector<Entity> entities;
		space.FindAllWith( nodes, entities );

		size_t partition_size = entities.size() / PARTITION_COUNT;

		size_t entity_start = 0;
		for( int partition = 0; partition < PARTITION_COUNT; ++partition )
		{
			size_t entity_count = partition_size;

			if( partition == 0 )
			{
				size_t remainder = entities.size() - partition_size * PARTITION_COUNT;
				entity_count = partition_size + remainder;
			}

			dd::Span<Entity> entity_span( entities, entity_count, entity_start );

			UpdateData data( space, entity_span, system.GetRequirements() );

			system.Update( data );

			data.Commit();

			entity_start += entity_count;
		}
	}

	struct SystemNode
	{
		struct Edge
		{
			SystemNode* m_node;
		};

		// nodes that are written to by systems that read this component
		int m_in { 0 };
		System* m_system { nullptr };
		std::vector<Edge> m_edges;
	};

	static void SortSystemsTopologically( std::vector<SystemNode>& nodes, std::vector<System*>& out_sorted )
	{
		out_sorted.clear();

		while( out_sorted.size() < nodes.size() )
		{
			for( SystemNode& node : nodes )
			{
				if( node.m_system != nullptr && node.m_in == 0 )
				{
					out_sorted.push_back( node.m_system );

					for( SystemNode::Edge& edge : node.m_edges )
					{
						edge.m_node->m_in--;
					}

					node.m_edges.clear();
					node.m_system = nullptr;
				}
			}
		}
	}

	void ScheduleSystemsByComponent( dd::Span<System*> systems, std::vector<System*>& out_ordered_systems )
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
							if( other_req->Component() == req->Component() &&
								other_req->Usage() == DataUsage::Read )
							{
								SystemNode::Edge edge;
								edge.m_node = &nodes[ other ];
								nodes[ sys ].m_edges.push_back( edge );

								nodes[ other ].m_in++;
							}
						}
					}
				}
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}

	void ScheduleSystemsByDependencies( dd::Span<System*> systems, std::vector<System*>& out_ordered_systems )
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
			for( size_t other = 0; other < deps.Size(); ++other )
			{
				const System* other_sys = systems[ other ];

				DD_ASSERT( system != other_sys, "Can't depend on the same system!" );

				SystemNode::Edge edge;
				edge.m_node = &nodes[ sys ];
				nodes[ other ].m_edges.push_back( edge );

				nodes[ sys ].m_in++;
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}
}