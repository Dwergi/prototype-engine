//
// SwarmAgentComponent.h - Component for swarm agents.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "DenseMapPool.h"
#include "Vector4.h"

namespace dd
{
	class SwarmAgentComponent : public Component
	{
	public:

		typedef DenseMapPool<SwarmAgentComponent> Pool;

		unsigned int ID;
		Vector4 Velocity;

		SwarmAgentComponent();
		virtual ~SwarmAgentComponent();

		BEGIN_TYPE( SwarmAgentComponent )
			PARENT( Component );
			MEMBER( SwarmAgentComponent, ID );
			MEMBER( SwarmAgentComponent, Velocity );
		END_TYPE
	};
}