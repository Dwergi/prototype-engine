#include "PrecompiledHeader.h"
#include "EntityLayer.h"

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

			UpdateData data( layer, entity_span, system.GetRequirements() );

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
							if( other_req->Component().ID == req->Component().ID &&
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