//
// SwarmSystem.h - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "SwarmAgentComponent.h"
#include "DoubleBuffer.h"

namespace dd
{
	class SwarmSystem
	{
	public:
		SwarmSystem( const DoubleBuffer<SwarmAgentComponent::Pool>& pool );
		~SwarmSystem();

		void Update( float dt );

		BASIC_TYPE( SwarmSystem )

	private:

		const DoubleBuffer<SwarmAgentComponent::Pool>& m_pool;
	};
}