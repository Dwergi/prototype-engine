//
// SystemsSorting.cpp
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#include "PCH.h"
#include "SystemsSorting.h"

#include "System.h"

namespace ddc
{
	size_t FindNodeIndex( const System* to_find, const std::vector<SystemNode>& nodes )
	{
		for( size_t i = 0; i < nodes.size(); ++i )
		{
			if( to_find == nodes[i].System )
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

				if( node.System != nullptr && node.In.size() == 0 )
				{
					out_sorted.push_back( nodes[n] );

					for( SystemNode::Edge& outEdge : node.Out )
					{
						SystemNode& dest_node = scratch[outEdge.To];

						size_t i = 0;
						for( ; i < dest_node.In.size(); ++i )
						{
							SystemNode::Edge& in_edge = dest_node.In[i];
							if( in_edge.From == n )
							{
								break;
							}
						}

						dest_node.In.erase( dest_node.In.begin() + i );
					}

					node.Out.clear();
					node.System = nullptr;
				}
			}
		}

		// remap node indices to new sorted ones
		std::vector<size_t> new_indices;
		new_indices.reserve( nodes.size() );

		for( const SystemNode& n : nodes )
		{
			new_indices.push_back( FindNodeIndex( n.System, out_sorted ) );
		}

		for( SystemNode& node : out_sorted )
		{
			for( SystemNode::Edge& edge : node.In )
			{
				edge.From = new_indices[edge.From];
				edge.To = new_indices[edge.To];
			}

			for( SystemNode::Edge& edge : node.Out )
			{
				edge.From = new_indices[edge.From];
				edge.To = new_indices[edge.To];
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
			node.System = system;
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
				edge.From = dep_index;
				edge.To = s;

				DD_ASSERT( edge.From != edge.To );

				nodes[dep_index].Out.push_back( edge );
				nodes[s].In.push_back( edge );
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}
}