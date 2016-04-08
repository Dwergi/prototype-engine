//
// SwarmSystem.h - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "SwarmAgentComponent.h"
#include "DoubleBuffer.h"
#include "ISystem.h"
#include "AABBOctree.h"

namespace dd
{
	class SwarmSystem : public ISystem
	{
	public:
		SwarmSystem();
		virtual ~SwarmSystem();

		virtual void Update( float dt ) override;

		BASIC_TYPE( SwarmSystem )

	private:

		DoubleBuffer<AABBOctree> m_agentsTree;
	};
}