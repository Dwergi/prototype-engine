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
		: m_agentsTree( new AABBOctree(), new AABBOctree() )
	{

	}

	SwarmSystem::~SwarmSystem()
	{

	}

	void SwarmSystem::Update( float dt )
	{
		const TransformComponent::Pool& transform_read = Services::GetReadPool<TransformComponent>();
		for( const TransformComponent& cmp : transform_read )
		{
			
		}

		const SwarmAgentComponent::Pool& swarm_read = Services::GetReadPool<SwarmAgentComponent>();
		for( const SwarmAgentComponent& cmp : swarm_read )
		{

		}

		AABBOctree& octree_write = m_agentsTree.GetWrite();
		octree_write.Clear();

		SwarmAgentComponent::Pool& swarm_write = Services::GetWritePool<SwarmAgentComponent>();
		for( SwarmAgentComponent& cmp : swarm_write )
		{
			cmp.OctreeHandle = octree_write.Add( glm::vec3() );
		}
	}
}