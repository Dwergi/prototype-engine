//
// SwarmSystem.cpp - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "SwarmSystem.h"

#include "DoubleBuffer.h"

namespace dd
{
	SwarmSystem::SwarmSystem( const DoubleBuffer<SwarmAgentComponent::Pool>& pool )
		: m_pool( pool )
	{

	}

	SwarmSystem::~SwarmSystem()
	{

	}

	void SwarmSystem::Update( float dt )
	{
		auto& read = m_pool.GetRead();
		for( const SwarmAgentComponent& cmp : read )
		{

		}
	}
}