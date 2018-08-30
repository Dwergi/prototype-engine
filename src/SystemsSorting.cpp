#include "PrecompiledHeader.h"
#include "SystemsSorting.h"

#include "System.h"

namespace ddc
{

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
						SystemNode& destNode = scratch[outEdge.m_to];

						size_t i = 0;
						for( ; i < destNode.m_in.size(); ++i )
						{
							SystemNode::Edge& inEdge = destNode.m_in[i];
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
			const System* system = systems[sys];

			const dd::IArray<const System*>& deps = system->GetDependencies();
			for( size_t dep = 0; dep < deps.Size(); ++dep )
			{
				DD_ASSERT( system != systems[dep], "Can't depend on the same system!" );

				SystemNode::Edge edge;
				edge.m_from = dep;
				edge.m_to = sys;

				nodes[dep].m_out.push_back( edge );
				nodes[sys].m_in.push_back( edge );
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
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

		for( size_t sys = 0; sys < systems.Size(); ++sys )
		{
			System* system = systems[sys];

			for( const DataRequirement* req : system->GetRequirements() )
			{
				if( req->Usage() == DataUsage::Write )
				{
					for( size_t other = 0; other < systems.Size(); ++other )
					{
						if( sys == other )
							continue;

						System* other_system = systems[other];

						for( const DataRequirement* other_req : other_system->GetRequirements() )
						{
							if( other_req->Component().ID == req->Component().ID &&
								other_req->Usage() == DataUsage::Read )
							{
								SystemNode::Edge edge;
								edge.m_from = sys;
								edge.m_to = other;

								nodes[sys].m_out.push_back( edge );
								nodes[other].m_in.push_back( edge );
							}
						}
					}
				}
			}
		}

		SortSystemsTopologically( nodes, out_ordered_systems );
	}
}