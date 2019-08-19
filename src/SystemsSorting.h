#pragma once

namespace ddc
{
	struct System;

	struct SystemNode
	{
		struct Edge
		{
			size_t m_from;
			size_t m_to;
		};

		System* m_system { nullptr };

		std::vector<Edge> m_in;
		std::vector<Edge> m_out;
	};

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
	void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
}