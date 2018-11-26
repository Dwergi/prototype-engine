//
// SwarmSystem.cpp - System to handle swarm simulation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "SwarmSystem.h"

#include "BoundBoxComponent.h"
#include "ColourComponent.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "PlayerComponent.h"
#include "Random.h"
#include "RayComponent.h"
#include "TransformComponent.h"

#include "glm/gtx/vector_angle.hpp"

namespace dd
{
	static int AgentCount = 100;

	// what radius the swarm searches for others in
	static float SearchRadius = 50.0f; 

	// what radius the swarm avoids each other in
	static float AvoidRadius = 10.0f;

	// how heavily to weight avoidance
	static float AvoidWeight = 0.6f;

	// the maximum amount of randomness to add
	static float MaxRandom = 0.1f;

	// how quickly the agent turns towards the average of surrounding agents
	// in radians per second
	static float TurnSpeed = 1.0f;

	// maximum speed in meters per second
	static float MaxSpeed = 10.0f;

	// minimum speed in meters per second
	static float MinSpeed = 4.0f;

	// how heavily the centre-seeking is weighted
	static float CentreWeight = 0.2f;

	// how heavily the direction cohesion is weighted
	static float CohesionWeight = 0.5f;

	// maximum acceleration rate in meters per second per second
	static float AccelerationRate = 1.0f;

	// how heavily the player-seeking is weighted
	static float PlayerSearchWeight = 0.3f;

	SwarmSystem::SwarmSystem() :
		ddc::System( "Swarm System" )
	{
		RequireWrite<TransformComponent>();
		RequireWrite<SwarmAgentComponent>();
		RequireWrite<RayComponent>();

		RequireRead<TransformComponent>( "player" );
		RequireRead<PlayerComponent>( "player" );
	}

	SwarmSystem::~SwarmSystem()
	{

	}

	void SwarmSystem::Initialize( ddc::World& world )
	{
		dd::RandomFloat rng( 0, 1 );

		for( size_t i = 0; i < AgentCount; ++i )
		{
			ddc::Entity entity = world.CreateEntity<dd::TransformComponent, dd::SwarmAgentComponent, dd::MeshComponent, dd::RayComponent, dd::BoundBoxComponent, dd::ColourComponent>();

			dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );
			transform->Position = glm::vec3( 50 * rng.Next(), 50 * rng.Next(), 50 * rng.Next() );
			transform->Rotation = glm::normalize( glm::quat( rng.Next(), rng.Next(), rng.Next(), rng.Next() ) );
			transform->Scale = glm::vec3( 0.25f );
			transform->Update();

			dd::BoundBoxComponent* bb = world.Access<dd::BoundBoxComponent>( entity );
			bb->BoundBox.Expand( glm::vec3( -0.25f ) );
			bb->BoundBox.Expand( glm::vec3( 0.25f ) );
			
			dd::ColourComponent* colour = world.Access<dd::ColourComponent>( entity );
			colour->Colour = glm::vec4( 1, 0, 0, 1 );

			dd::SwarmAgentComponent* agent = world.Access<dd::SwarmAgentComponent>( entity );
			agent->Velocity = transform->Rotation * glm::vec3( 0, 0, 10 * rng.Next() );

			dd::MeshComponent* mesh = world.Access<dd::MeshComponent>( entity );
			mesh->Mesh = ddr::MeshManager::Instance()->Find( "cube" );

			dd::RayComponent* ray = world.Access<dd::RayComponent>( entity );
			ray->Ray = Ray( glm::vec3( 0 ), glm::vec3( 0, 0, 1 ), 0.5f );

			world.AddTag( entity, ddc::Tag::Visible );
		}
	}

	void SwarmSystem::Update( const ddc::UpdateData& update )
	{
		const float dt = update.Delta();
		const float max_turn = TurnSpeed * dt;
		const float max_accel = AccelerationRate * dt;

		const ddc::DataBuffer& data = update.Data();
		auto transforms = data.Write<TransformComponent>();
		auto swarm_agents = data.Write<SwarmAgentComponent>();
		auto rays = data.Write<RayComponent>();

		const ddc::DataBuffer& player_data = update.Data( "player" );
		auto player_transforms = player_data.Read<TransformComponent>();

		glm::vec3 player_pos = player_transforms[0].Position;

		m_agentsBVH.Clear();
		m_agentsBVH.StartBatch();

		// fill BVH
		for( size_t i = 0; i < data.Size(); ++i )
		{
			const TransformComponent& transform = transforms[i];

			dd::AABB bb;
			bb.Expand( transform.Position );

			m_agentsBVH.Add( bb );
		}

		m_agentsBVH.EndBatch();

		RandomFloat rng( -MaxRandom, MaxRandom );

		std::vector<size_t> temp_entries;
		temp_entries.reserve( data.Size() );

		for( size_t i = 0; i < data.Size(); ++i )
		{
			TransformComponent& transform = transforms[i];
			SwarmAgentComponent& swarm_agent = swarm_agents[i];

			// find all entries within the search radius
			AABB bounds;
			bounds.Expand( transform.Position + glm::vec3( SearchRadius ) );
			bounds.Expand( transform.Position - glm::vec3( SearchRadius ) );
			
			temp_entries.clear();
			m_agentsBVH.WithinBoundBox( bounds, temp_entries );
			
			uint friend_count = 0;
			glm::vec3 pos_centre( 0 );
			glm::vec3 dir_average( 0 );

			float spd_average = 0;

			uint avoid_count = 0;
			glm::vec3 dir_avoid( 0 );

			// calculate the average of velocities and positions of surrounding agents
			for( size_t entry : temp_entries )
			{
				// skip self
				if( entry == i )
					continue;

				DD_ASSERT( entry >= 0 && entry < transforms.Size() );

				const SwarmAgentComponent& other_agent = swarm_agents[entry];
				const TransformComponent& other_transform = transforms[entry];

				++friend_count;

				float distance = glm::distance( transform.Position, other_transform.Position );
				
				float spd = glm::length( other_agent.Velocity );
				spd_average += spd;
				dir_average += (other_agent.Velocity / spd) / distance;
				pos_centre += other_transform.Position;

				if( distance < AvoidRadius )
				{
					dir_avoid += glm::normalize( transform.Position - other_transform.Position ) / distance;
					++avoid_count;
				}
			}

			if( friend_count > 0 )
			{
				dir_average /= friend_count;
				pos_centre /= friend_count;
				dir_avoid /= avoid_count;

				// create velocity towards centre of swarm
				glm::vec3 dir_centre = glm::normalize( pos_centre - transform.Position );
				glm::vec3 dir_player = glm::normalize( player_pos - transform.Position );
				dir_average = glm::normalize( dir_average );

				float total_weight = 0;
				glm::vec3 dir_desired( 0 );
				
				dir_desired += CentreWeight * dir_centre;
				total_weight += CentreWeight;

				dir_desired += PlayerSearchWeight * dir_player;
				total_weight += PlayerSearchWeight;

				if( avoid_count > 0 )
				{
					dir_desired += AvoidWeight * dir_avoid;
					total_weight += AvoidWeight;
				}

				dir_desired += total_weight * dir_average;
				dir_desired /= total_weight;

				dir_desired += glm::vec3( rng.Next(), rng.Next(), rng.Next() );

				float spd_current = glm::length( swarm_agent.Velocity );

				glm::vec3 dir_current = swarm_agent.Velocity / spd_current;

				float angle = glm::angle( dir_current, glm::normalize( dir_desired ) );
				glm::vec3 dir_final = glm::mix( dir_current, dir_desired, ddm::min( 1.0f, max_turn / angle ) );

				float spd_desired = spd_average + rng.Next();
				spd_desired = glm::clamp( spd_desired, spd_current - max_accel, spd_current + max_accel );

				float spd_final = glm::clamp( spd_desired, MinSpeed, MaxSpeed );

				swarm_agent.Velocity = dir_final * spd_final;
			}

			float pitch, yaw;
			ddm::PitchYawFromDirection( swarm_agent.Velocity, pitch, yaw );

			transform.Rotation = ddm::QuatFromPitchYaw( pitch, yaw );
			transform.Position = transform.Position + swarm_agent.Velocity * dt;
			transform.Update();

			rays[i].Ray = Ray( transform.Position, swarm_agent.Velocity, glm::length( swarm_agent.Velocity ) );
		}
	}

	void SwarmSystem::DrawDebugInternal( ddc::World& world )
	{
		ImGui::SliderInt( "Agents", &AgentCount, 0, 10000 );

		ImGui::SliderFloat( "Search Radius", &SearchRadius, 0, 500 );

		ImGui::SliderFloat( "Centre Weight", &CentreWeight, 0, 1 );

		ImGui::SliderFloat( "Cohesion Weight", &CohesionWeight, 0, 1 );

		ImGui::SliderFloat( "Avoid Radius", &AvoidRadius, 0, SearchRadius );

		ImGui::SliderFloat( "Max Random", &MaxRandom, 0, 1 );

		ImGui::SliderFloat( "Turn Speed", &TurnSpeed, 0, 1 );

		ImGui::DragFloatRange2( "Speed Range", &MinSpeed, &MaxSpeed, 0.01, 0, 50 );

		ImGui::SliderFloat( "Acceleration", &AccelerationRate, 0, 10 );

		ImGui::SliderFloat( "Player Weight", &PlayerSearchWeight, 0, 1 );
	}
}