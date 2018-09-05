#pragma once

#include <future>

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

		std::shared_future<void> m_update;
	};

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
	void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
}