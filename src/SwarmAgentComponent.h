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

		/*BEGIN_MEMBERS( dd::SwarmAgentComponent )
			MEMBER( dd::SwarmAgentComponent, unsigned int, ID, "ID" );
			MEMBER( dd::SwarmAgentComponent, Vector4, Velocity, "Velocity" );
		END_MEMBERS*/
	};
}