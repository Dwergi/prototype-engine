//
// SwarmAgentComponent.h - Component for swarm agents.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Component.h"
#include "DenseMapPool.h"
#include "Vector4.h"
#include "AABBOctree.h"

namespace dd
{
	class SwarmAgentComponent : public Component
	{
	public:

		typedef DenseMapPool<SwarmAgentComponent> Pool;

		glm::vec3 Velocity;
		OctreeEntry OctreeHandle;

		SwarmAgentComponent();
		virtual ~SwarmAgentComponent();

		BEGIN_SCRIPT_OBJECT( SwarmAgentComponent )
			PARENT( Component )
			MEMBER( SwarmAgentComponent, Velocity )
			MEMBER( SwarmAgentComponent, OctreeHandle )
		END_TYPE
	};
}