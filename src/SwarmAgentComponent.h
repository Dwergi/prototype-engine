//
// SwarmAgentComponent.h - Component for swarm agents.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "UnorderedMapPool.h"
#include "Vector4.h"

namespace dd
{
	class SwarmAgentComponent : public dd::Component
	{
	public:

		typedef dd::UnorderedMapPool<SwarmAgentComponent> Pool;

		unsigned int ID;
		dd::Vector4 Velocity;

		SwarmAgentComponent();
		virtual ~SwarmAgentComponent();

		BEGIN_MEMBERS
			MEMBER( dd::SwarmAgentComponent, ID );
			MEMBER( dd::SwarmAgentComponent, Velocity );
		END_MEMBERS
	};
}