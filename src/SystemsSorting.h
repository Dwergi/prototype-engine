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
			size_t From { 0 };
			size_t To { 0 };
		};

		System* System { nullptr };
		dd::Job* UpdateJob { nullptr };

		std::vector<Edge> In;
		std::vector<Edge> Out;
	};

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_systems );
}