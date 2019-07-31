//
// TreeSystem.cpp - System for updating trees.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "TreeSystem.h"

#include "BoundBoxComponent.h"
#include "LinesComponent.h"
#include "MeshComponent.h"
#include "Random.h"
#include "TransformComponent.h"
#include "TreeComponent.h"

#include "glm/gtc/noise.hpp"

namespace dd
{
	static float StartingLifetime = 4;
	static glm::vec3 VelocityDecay { 0.5, 0.7, 0.5 };
	static glm::vec3 Gravity { 0, -9.81, 0 };
	static float GravityInfluence = 0.01;

	static float BranchingStart = 0.25;
	static float BranchingEnd = 0.6;
	static float BranchingGrowth = 1.0;

	static float BranchAngleMin = 0.5f;
	static float BranchAngleMax = glm::pi<float>();

	static float LineInterval = 0.16f;

	static glm::vec3 StartingVelocity = glm::vec3( 0, 30, 0 );
	static glm::vec3 RandomMin = glm::vec3( -1.0 );
	static glm::vec3 RandomMax = glm::vec3( 1.0 );

	TreeSystem::TreeSystem() :
		ddc::System( "Trees" )
	{
		RequireRead<dd::TransformComponent>();
		RequireWrite<dd::TreeComponent>();
		RequireWrite<dd::BoundBoxComponent>();
		RequireWrite<dd::LinesComponent>();
	}

	static void InitializeTree( dd::TreeComponent& tree )
	{
		tree.Particles.clear();

		TreeParticle& initial = tree.Particles.emplace_back();
		initial.Lifetime = StartingLifetime;
		initial.Velocity = StartingVelocity;
		initial.BranchChance = BranchingStart;
		initial.BranchAccumulator = 0;
		initial.Position = glm::vec3( 0 );
	}

	void TreeSystem::Initialize( ddc::World& world )
	{
		ddc::Entity entity = world.CreateEntity();

		dd::TransformComponent& transform = world.Add<dd::TransformComponent>( entity );
		transform.Position = glm::vec3( 10, 0, 10 );
		transform.Update();

		dd::BoundBoxComponent& bound_box = world.Add<dd::BoundBoxComponent>( entity );
		bound_box.BoundBox.Expand( transform.Position );

		dd::LinesComponent& line_cmp = world.Add<dd::LinesComponent>( entity );
		
		dd::TreeComponent& tree = world.Add<dd::TreeComponent>( entity );
		InitializeTree( tree );
	}

	static void BranchParticle( dd::TreeComponent& tree, dd::TreeParticle& particle )
	{
		if( tree.Particles.size() > 1024 )
			return;

		dd::TreeParticle& branched = tree.Particles.emplace_back();
		branched.BranchChance = particle.BranchChance;
		branched.BranchAccumulator = 0;
		branched.Lifetime = particle.Lifetime;

		branched.PreviousLine = particle.Position;
		branched.Position = particle.Position;

		// rotation around the direction
		glm::vec3 dir = glm::normalize( particle.Velocity );
		glm::quat dir_rotation = glm::normalize( glm::angleAxis( glm::two_pi<float>() * tree.RNG.Next(), dir ) );

		// rotation downwards
		glm::vec3 axis = glm::cross( glm::vec3( 1, 0, 0 ), dir );
		if( ddm::IsNaN( axis ) )
		{
			axis = glm::vec3( 0, 0, 1 );
		}

		glm::quat dir_down = glm::normalize( glm::angleAxis( glm::mix( BranchAngleMin, BranchAngleMin, tree.RNG.Next() ), glm::normalize( axis ) ) );

		branched.Velocity = dir_rotation * dir_down * particle.Velocity;
	}

	static void UpdateTree( dd::TreeComponent& tree, dd::BoundBoxComponent& bound_box, dd::LinesComponent& lines, const dd::TransformComponent& transform, const float delta_t )
	{
		bound_box.BoundBox.Clear();
		bound_box.BoundBox.Expand( transform.Position );

		int alive = 0;
		for( TreeParticle& p : tree.Particles )
		{
			p.Lifetime -= delta_t;

			if( p.Lifetime <= 0 )
			{
				continue;
			}

			float noise_influence = 1 - p.Lifetime / StartingLifetime;
			p.Velocity = p.Velocity - (p.Velocity * VelocityDecay * (1 - noise_influence) * delta_t) 
				+ glm::vec3( glm::simplex( p.Velocity ) ) * noise_influence * delta_t;

			p.Velocity += Gravity * GravityInfluence * delta_t;

			p.Position += p.Velocity * delta_t;
			
			p.LineAccumulator += delta_t;
			if( p.LineAccumulator > LineInterval )
			{
				lines.Points.Add( p.PreviousLine );
				lines.Points.Add( p.Position );

				p.PreviousLine = p.Position;
				p.LineAccumulator = 0;
			}

			p.BranchChance += glm::clamp( BranchingGrowth * delta_t, 0.0f, 1.0f );

			if( tree.RNG.Next() < p.BranchChance * delta_t )
			{
				BranchParticle( tree, p );

				++alive;
			}

			glm::vec4 world_pos = transform.Transform() * glm::vec4( p.Position, 1 );
			bound_box.BoundBox.Expand( world_pos.xyz );

			++alive;
		}

		if( alive == 0 )
		{
			InitializeTree( tree );
			bound_box.BoundBox.Clear();
			bound_box.BoundBox.Expand( transform.Position );

			lines.Points.Clear();
			return;
		}
	}

	void TreeSystem::Update( const ddc::UpdateData& update_data )
	{
		const float delta_t = update_data.Delta();

		auto data = update_data.Data();
		
		auto trees = data.Write<dd::TreeComponent>();
		auto bounds = data.Write<dd::BoundBoxComponent>();
		auto transforms = data.Read<dd::TransformComponent>();
		auto lines = data.Write<dd::LinesComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			UpdateTree( trees[i], bounds[i], lines[i], transforms[i], delta_t );
		}
	}

	void TreeSystem::DrawDebugInternal( ddc::World& world )
	{
		ImGui::DragFloat( "Lifetime", &StartingLifetime, 0.01, 0, 20 );

		ImGui::DragFloat3( "Velocity", glm::value_ptr( StartingVelocity ), 0.1, 0, 100 );

		ImGui::DragFloat3( "Velocity Decay", glm::value_ptr( VelocityDecay ), 0.01, 0, 1 );

		ImGui::DragFloat( "Line Interval", &LineInterval, 0.001, 1.0f / 60, 1 );

		if( ImGui::TreeNodeEx( "Branching", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloat( "Start Chance", &BranchingStart, 0.001, 0, 1.0 );

			ImGui::DragFloat( "End Chance", &BranchingEnd, 0.001, 0, 1.0 );

			ImGui::DragFloat( "Growth Rate", &BranchingGrowth, 0.001, -1.0, 1.0 );

			ImGui::DragFloatRange2( "Angle Range", &BranchAngleMin, &BranchAngleMax, 0.001 );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Randomization", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &RandomMin.x, &RandomMax.x, 0.001f, -5, 5 );
			ImGui::DragFloatRange2( "Y", &RandomMin.y, &RandomMax.y, 0.001f, -5, 5 );
			ImGui::DragFloatRange2( "Z", &RandomMin.y, &RandomMax.z, 0.001f, -5, 5 );

			ImGui::TreePop();
		}
	}
}