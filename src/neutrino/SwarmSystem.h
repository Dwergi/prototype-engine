//
// SwarmSystem.h - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "BVHTree.h"
#include "IDebugPanel.h"
#include "System.h"
#include "neutrino/SwarmAgentComponent.h"

namespace neut
{
	struct SwarmSystem : ddc::System, dd::IDebugPanel
	{
		SwarmSystem();
		virtual ~SwarmSystem();

		virtual void Initialize( ddc::EntitySpace& entities ) override;

		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Swarm"; }

		DD_BASIC_TYPE( neut::SwarmSystem );

	private:

		dd::BVHTree m_agentsBVH;

		virtual void DrawDebugInternal() override;
	};
}