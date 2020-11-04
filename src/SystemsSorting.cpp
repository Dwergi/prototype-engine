//
// SystemsSorting.cpp
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#include "PCH.h"
#include "SystemsSorting.h"

#include "ddc/System.h"

namespace ddc
{
	size_t FindNodeIndex( const System* to_find, const std::vector<SystemNode>& nodes )
	{
		for( size_t i = 0; i < nodes.size(); ++i )
		{
			if( to_find == nodes[i].m_system )
			{
				return i;
			}
		}

		DD_ASSERT( "Not found, all is doomed." );
		return ~0;
	}

	void SortSystemsTopologically( const std::vector<SystemNode>& nodes, std::vector<SystemNode>& out_sorted )
	{
		std::vector<SystemNode> scratch = nodes;

		out_sorted.clear();

		while( out_sorted.size() < nodes.size() )
		{
			for( size_t n = 0; n < scratch.size(); ++n )
			{
				SystemNode& node = scratch[n];

				if( node.m_system != nullptr && node.m_in.size() == 0 )
				{
					out_sorted.push_back( nodes[n] );

					for( SystemNode::Edge& outEdge : node.m_out )
					{
						SystemNode& dest_node = scratch[outEdge.m_to];

						size_t i = 0;
						for( ; i < dest_node.m_in.size(); ++i )
						{
							SystemNode::Edge& in_edge = dest_node.m_in[i];
							if( in_edge.m_from == n )
							{
								break;
							}
						}

						dest_node.m_in.erase( dest_node.m_in.begin() + i );
					}

					node.m_out.clear();
					node.m_system = nullptr;
				}
			}
		}

		// remap node indices to new sorted ones
		std::vector<size_t> new_indices;
		new_indices.reserve( nodes.size() );

		for( const SystemNode& n : nodes )
		{
			new_indices.push_back( FindNodeIndex( n.m_system, out_sorted ) );
		}

		for( SystemNode& node : out_sorted )
		{
			for( SystemNode::Edge& edge : node.m_in )
			{
				edge.m_from = new_indices[edge.m_from];
				edge.m_to = new_indices[edge.m_to];
			}

			for( SystemNode::Edge& edge : node.m_out )
			{
				edge.m_from = new_indices[edge.m_from];
				edge.m_to = new_indices[edge.m_to];
			}
		}
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

		for( size_t s = 0; s < systems.Size(); ++s )
		{
			const System* system = systems[s];
			for( const System* dep : system->GetDependencies() )
			{
				DD_ASSERT( system != dep, "Can't depend on the same system!" );

				size_t dep_index = FindNodeIndex( dep, nodes );

				SystemNode::Edge edge;
				edge.m_from = dep_index;
				edge.m_to = s;

				DD_ASSERT( edge.m_from != edge.m_to );

				nodes[dep_index].m_out.push_back( edge );
				nodes[s].m_in.push_back( edge );
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}
}