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

namespace dd
{
	static float StartingLifetime = 5;
	static float DecayFactor = 1;
	static float BranchingStart = 0;
	static float BranchingEnd = 1.0;
	static float BranchingGrowth = 0.05;

	static glm::vec3 StartingVelocity = glm::vec3( 0, 100, 0 );
	static glm::vec3 RandomMin = glm::vec3( -1.0 );
	static glm::vec3 RandomMax = glm::vec3( 1.0 );

	const glm::vec3 Gravity = glm::vec3( 0, -9.81, 0 );

	TreeSystem::TreeSystem() :
		ddc::System( "Trees" )
	{
		RequireRead<dd::TransformComponent>();
		RequireWrite<dd::TreeComponent>();
		RequireWrite<dd::BoundBoxComponent>();
	}

	static void InitializeTree( dd::TreeComponent& tree )
	{
		tree.Particles.clear();

		TreeParticle& initial = tree.Particles.emplace_back();
		initial.Lifetime = StartingLifetime;
		initial.Velocity = StartingVelocity;
		initial.BranchChance = BranchingStart;
		initial.Position = glm::vec3( 0 );
	}

	void TreeSystem::Initialize( ddc::World& world )
	{
		ddc::Entity entity = world.CreateEntity();

		dd::TransformComponent& transform = world.Add<dd::TransformComponent>( entity );

		dd::BoundBoxComponent& bound_box = world.Add<dd::BoundBoxComponent>( entity );
		bound_box.BoundBox.Expand( transform.Position );

		dd::LinesComponent& line_cmp = world.Add<dd::LinesComponent>( entity );
		
		dd::TreeComponent& tree = world.Add<dd::TreeComponent>( entity );
		InitializeTree( tree );
	}

	static void UpdateTree( dd::TreeComponent& tree, dd::BoundBoxComponent& bound_box, dd::LinesComponent& lines, const dd::TransformComponent& transform, const float delta_t )
	{
		bound_box.BoundBox.Clear();

		int alive = 0;
		for( TreeParticle& p : tree.Particles )
		{
			p.Velocity += Gravity * delta_t + dd::GetRandomVector3( tree.RNG, RandomMin, RandomMax ) * delta_t;
			p.Position += p.Velocity * delta_t;

			lines.Points.push_back( p.Position );

			glm::vec4 world_pos = transform.Transform() * glm::vec4( p.Position, 1 );
			bound_box.BoundBox.Expand( world_pos.xyz );
		}

		if( alive == 0 )
		{
			InitializeTree( tree );
			bound_box.BoundBox.Clear();
			bound_box.BoundBox.Expand( transform.Position );
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

		ImGui::DragFloat3( "Velocity", glm::value_ptr( StartingVelocity ), 0.01, 0, 100 );

		ImGui::DragFloat( "Decay Multiplier", &DecayFactor, 0.01, 0, 5 );

		ImGui::DragFloat( "Branching Start", &BranchingStart, 0.001, 0, 1.0 );

		ImGui::DragFloat( "Branching End", &BranchingEnd, 0.001, 0, 1.0 );

		ImGui::DragFloat( "Branching Growth", &BranchingGrowth, 0.001, -1.0, 1.0 );

		if( ImGui::TreeNodeEx( "Randomization", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &RandomMin.x, &RandomMax.x, 0.001f, -5, 5 );
			ImGui::DragFloatRange2( "Y", &RandomMin.y, &RandomMax.y, 0.001f, -5, 5 );
			ImGui::DragFloatRange2( "Z", &RandomMin.y, &RandomMax.z, 0.001f, -5, 5 );

			ImGui::TreePop();
		}
	}
}