//
// SwarmAgentComponent.h - Component for swarm agents.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "AABBOctree.h"

namespace dd
{
	class SwarmAgentComponent
	{
	public:

		glm::vec3 Velocity;

		DD_BEGIN_CLASS( dd::SwarmAgentComponent )
			DD_COMPONENT();

			DD_MEMBER( Velocity );
		DD_END_CLASS()
	};
}