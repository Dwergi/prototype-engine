//
// SwarmSystem.h - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "SwarmAgentComponent.h"
#include "DoubleBuffer.h"
#include "System.h"
#include "AABBOctree.h"

namespace dd
{
	class SwarmSystem : public ddc::System
	{
	public:
		SwarmSystem();
		virtual ~SwarmSystem();

		virtual void Update( const ddc::UpdateData& data, float dt ) override;

		DD_BASIC_TYPE( SwarmSystem )

	private:

		DoubleBuffer<AABBOctree> m_agentsTree;
	};
}