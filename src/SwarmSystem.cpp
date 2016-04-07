//
// SwarmSystem.cpp - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "SwarmSystem.h"

#include "DoubleBuffer.h"
#include "TransformComponent.h"

namespace dd
{
	SwarmSystem::SwarmSystem()
	{

	}

	SwarmSystem::~SwarmSystem()
	{

	}

	void SwarmSystem::Update( float dt )
	{
		const TransformComponent::Pool& transform_read = Services::GetReadPool<TransformComponent>();
		const SwarmAgentComponent::Pool& swarm_read = Services::GetReadPool<SwarmAgentComponent>();
		for( const SwarmAgentComponent& cmp : swarm_read )
		{

		}
	}
}