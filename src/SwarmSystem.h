//
// SwarmSystem.h - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "BVHTree.h"
#include "IDebugPanel.h"
#include "SwarmAgentComponent.h"
#include "System.h"

namespace dd
{
	struct SwarmSystem : ddc::System, dd::IDebugPanel
	{
		SwarmSystem();
		virtual ~SwarmSystem();

		virtual void Initialize( ddc::World& world ) override;

		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Swarm"; }

		DD_BASIC_TYPE( SwarmSystem );

	private:

		BVHTree m_agentsBVH;

		virtual void DrawDebugInternal( ddc::World& world ) override;
	};
}