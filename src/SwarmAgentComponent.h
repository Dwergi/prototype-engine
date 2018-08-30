//
// SwarmAgentComponent.h - Component for swarm agents.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "ComponentType.h"
#include "Vector4.h"
#include "AABBOctree.h"

namespace dd
{
	class SwarmAgentComponent
	{
	public:

		glm::vec3 Velocity;
		OctreeEntry OctreeHandle;

		SwarmAgentComponent();
		virtual ~SwarmAgentComponent();

		DD_COMPONENT;

		DD_SCRIPT_OBJECT( SwarmAgentComponent )
			DD_MEMBER( SwarmAgentComponent, Velocity )
			DD_MEMBER( SwarmAgentComponent, OctreeHandle )
		DD_END_TYPE
	};
}