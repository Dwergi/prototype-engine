//
// SystemsSorting.h
// Copyright (C) Sebastian Nordgren 
// August 21st 2019
//

#pragma once

namespace dd
{
	struct Job;
}

namespace ddc
{
	struct System;

	struct SystemNode
	{
		struct Edge
		{
			size_t m_from { 0 };
			size_t m_to { 0 };
		};

		System* m_system { nullptr };
		dd::Job* m_job { nullptr };

		std::vector<Edge> m_in;
		std::vector<Edge> m_out;
	};

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
	void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
}