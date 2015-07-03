#pragma once

#include "SwarmAgentComponent.h"
#include "DoubleBuffer.h"

namespace dd
{
	class SwarmSystem
	{
	public:
		SwarmSystem( const dd::DoubleBuffer< dd::SwarmAgentComponent::Pool >& pool );
		~SwarmSystem();

		void Update( float dt );

	private:

		const dd::DoubleBuffer< dd::SwarmAgentComponent::Pool >& m_pool;
	};
}