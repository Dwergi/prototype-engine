//
// SwarmSystem.cpp - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "SwarmSystem.h"

#include "PlayerComponent.h"
#include "TransformComponent.h"

namespace dd
{
	// what radius the 
	const float SearchRadius = 50.0f; 

	// how quickly the agent turns towards the average of surrounding agents
	// in percentage per second - higher is faster
	// 1.0 means that it will match the average in one second
	const float TurnSpeed = 1.0f; 

	// maximum speed in meters per second
	const float MaxSpeed = 10.0f;

	// minimum speed in meters per second
	const float MinSpeed = 4.0f;

	// how heavily the centre-seeking is weighted
	const float CentreWeight = 0.1f;

	// maximum acceleration rate in meters per second per second
	const float AccelerationRate = 1.0f;

	SwarmSystem::SwarmSystem() :
		ddc::System( "Swarm System" ),
		m_agentsTree( new AABBOctree(), new AABBOctree() )
	{

	}

	SwarmSystem::~SwarmSystem()
	{

	}

	void SwarmSystem::Update( const ddc::UpdateData& data, float dt )
	{
		DD_TODO( "Reimplement swarm system" );
		/*float turn_multiplier = TurnSpeed * dt;
		float accel_multiplier = AccelerationRate * dt;

		const TransformComponent::Pool& transforms_read = manager.ForAllWithReadable<TransformComponent>();
		const TransformComponent::Pool& transforms_write = Services::GetWritePool<TransformComponent>();

		SwarmAgentComponent::Pool& swarm_write = Services::GetWritePool<SwarmAgentComponent>();
		swarm_write.Clear();

		const AABBOctree& octree_read = m_agentsTree.Read();
		const SwarmAgentComponent::Pool& swarm_read = Services::GetReadPool<SwarmAgentComponent>();

		DenseMap<OctreeEntry, const SwarmAgentComponent*> entry_map;
		for( const SwarmAgentComponent& cmp_read : swarm_read )
		{
			entry_map.Add( cmp_read.OctreeHandle, &cmp_read );
		}

		AABBOctree& octree_write = m_agentsTree.Write();
		octree_write.Clear();

		Vector<OctreeEntry> temp_entries;
		for( const SwarmAgentComponent& cmp_read : swarm_read )
		{
			const TransformComponent* transform_read = transforms_read.Find( cmp_read.Entity );
			DD_ASSERT( transform_read != nullptr );

			temp_entries.Clear();

			// find all entries within the search radius
			AABB bounds;
			bounds.Expand( transform_read->GetPosition() );
			bounds.Expand( transform_read->GetPosition() + glm::vec3(  SearchRadius, SearchRadius, SearchRadius ) );
			bounds.Expand( transform_read->GetPosition() + glm::vec3( -SearchRadius, -SearchRadius, -SearchRadius ) );
			
			octree_read.GetAllIntersecting( bounds, temp_entries );

			uint count = 0;
			glm::vec3 centre( 0, 0, 0 );
			glm::vec3 velocity( 0, 0, 0 );

			// calculate the average of velocities and positions of surrounding agents
			for( OctreeEntry entry : temp_entries )
			{
				// skip self
				if( entry == cmp_read.OctreeHandle )
					continue;

				const SwarmAgentComponent** other_cmp = entry_map.Find( entry );
				DD_ASSERT( other_cmp != nullptr );

				const TransformComponent* other_transform = transforms_read.Find( (*other_cmp)->Entity );
				DD_ASSERT( other_transform != nullptr );

				velocity += (*other_cmp)->Velocity;
				centre += other_transform->GetPosition();

				++count;
			}

			dd::SwarmAgentComponent* cmp_write = swarm_write.Create( cmp_read.Entity );
			dd::TransformComponent* transform_write = transforms_write.Find( cmp_read.Entity );

			if( count > 0 )
			{
				velocity /= count;
				centre /= count;

				// create velocity towards centre of swarm
				glm::vec3 dir_centre = centre - transform_read->GetPosition();

				velocity = glm::mix( velocity, dir_centre, CentreWeight );
				velocity *= turn_multiplier;

				// modulate the current velocity towards the swarm's average
				if( glm::length( velocity ) > accel_multiplier )
				{
					velocity = glm::normalize( velocity ) * accel_multiplier;
				}

				cmp_write->Velocity = cmp_read.Velocity + velocity;
			}
			else
			{
				cmp_write->Velocity = cmp_read.Velocity; // just keep velocity the same
			}

			transform_write->SetPosition( transform_read->GetPosition() + (cmp_write->Velocity * dt) );
			cmp_write->OctreeHandle = octree_write.Add( transform_write->GetPosition() );
		}*/
	}
}