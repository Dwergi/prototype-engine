//
// ShipSystem.cpp - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "PCH.h"
#include "neutrino/ShipSystem.h"

#include "FPSCameraComponent.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "TransformComponent.h"

#include "neutrino/ShipComponent.h"

namespace neut
{
	static const float s_shipMesh[] =
	{
		// left edge
		0.0f,0.0f,1.0f,		-1.0f,0.0f,0.0f,
		-0.5f,0.0f,-1.0f,	-1.0f,0.0f,0.0f,
		0.0f,0.5f,-1.0f,	-1.0f,0.0f,0.0f,

		// back
		-0.5f,0.0f,-1.0f,	0.0f,0.0f,-1.0f,
		0.5f,0.0f,-1.0f,	0.0f,0.0f,-1.0f,
		0.0f,0.5f,-1.0f,	0.0f,0.0f,-1.0f,

		// right edge
		0.5f,0.0f,-1.0f,	1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,		1.0f,0.0f,0.0f,
		0.0f,0.5f,-1.0f,	1.0f,0.0f,0.0f,

		// bottom
		0.5f,0.0f,-1.0f,	0.0f,-1.0f,0.0f,
		0.0f,0.0f,1.0f,		0.0f,-1.0f,0.0f,
		-0.5f,0.0f,-1.0f,	0.0f,-1.0f,0.0f
	};

	static dd::Service<dd::Input> s_input;

	ShipSystem::ShipSystem() :
		ddc::System("Ship System")
	{
	}

	void ShipSystem::Update(const ddc::UpdateData& data)
	{
		if (!m_enabled)
			return;

		DD_TODO("Uncomment");

		/*	entities.ForAllWithReadable<TransformComponent, ShipComponent>( [this, dt]( auto entity, auto transform, auto ship )
			{
				UpdateShip( entity, transform, ship, dt );
			} );*/
	}

	void ShipSystem::CreateShip(ddc::EntitySpace& entities)
	{
		DD_TODO("Uncomment");
		/*
		EntityHandle entity = entities.CreateEntity<TransformComponent, MeshComponent, ShipComponent>();

		glm::mat4 transform = glm::translate( glm::vec3( 0, 2.5, 10 ) );

		TransformComponent* transform_cmp = entities.GetWritable<TransformComponent>( entity );
		transform_cmp->Local = transform;

		ddr::ShaderHandle shader = ddr::s_shaderManager->Load( "mesh" );
		/ *shader.Get()->Use( true );
		shader.Get()->BindAttributeFloat( "Position", 3, 6, 0, false );
		shader.Get()->BindAttributeFloat( "Normal", 3, 6, 3, false );
		shader.Get()->Use( false );* /

		AABB bounds;
		bounds.Expand( glm::vec3( 0, 0, 1 ) );
		bounds.Expand( glm::vec3( 0.5f, 0, -1.0 ) );
		bounds.Expand( glm::vec3( -0.5f, 0, -1.0 ) );

		m_shipMesh = s_meshManager->Create( "ship" );

		ddr::Mesh* mesh = ddr::Mesh::Get( m_shipMesh );
		mesh->SetMaterial(ddr::MaterialHandle( "mesh" ) );
		dd::MakeUnitCube( *mesh );

		/ *mesh_h.Get()->SetData( s_shipMesh, sizeof( s_shipMesh ), 6 );
		mesh_h.Get()->SetBoundBox( bounds );* /

		MeshComponent* mesh_cmp = entities.GetWritable<MeshComponent>( entity );
		mesh_cmp->Mesh = m_shipMesh;
		mesh_cmp->Colour = glm::vec4( 1, 0, 0, 1 );
		mesh_cmp->Hidden = false;

		ShipComponent* ship_cmp = entities.GetWritable<ShipComponent>( entity );
		ship_cmp->Acceleration = 5.0f;
		ship_cmp->Velocity = glm::vec3( 0, 0, 1 );
		ship_cmp->BoostFactor = 2.0f;
		ship_cmp->MaximumSpeed = 20.0f;
		ship_cmp->MinimumSpeed = 5.0f;
		ship_cmp->BoostMaximum = 2.5f;
		ship_cmp->BoostRemaining = ship_cmp->BoostMaximum;
		ship_cmp->BoostRechargeRate = 2.0f;

		m_lastShip = entity;*/
	}

	void ShipSystem::UpdateShip(dd::TransformComponent& transform, neut::ShipComponent& ship, float delta_t)
	{
		glm::vec3 current_velocity = ship.Velocity;
		float current_speed = glm::length(current_velocity);
		glm::vec3 current_position = transform.Position;
		bool boosting = false;

		glm::vec3 other_modifiers = glm::vec3(0, 0, 0);

		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = glm::normalize(glm::cross(current_velocity, up));

		if (s_input->IsHeld(dd::InputAction::FORWARD))
			current_velocity *= 1.0f + (ship.Acceleration * delta_t);

		if (s_input->IsHeld(dd::InputAction::BACKWARD))
			current_velocity *= 1.0f - (ship.Acceleration * delta_t);

		if (s_input->IsHeld(dd::InputAction::UP))
			other_modifiers += up * current_speed * delta_t;

		if (s_input->IsHeld(dd::InputAction::DOWN))
			other_modifiers -= up * current_speed * delta_t;

		if (s_input->IsHeld(dd::InputAction::RIGHT))
			other_modifiers += right * current_speed * delta_t;

		if (s_input->IsHeld(dd::InputAction::LEFT))
			other_modifiers -= right * current_speed * delta_t;

		if (s_input->IsHeld(dd::InputAction::BOOST) && ship.BoostRemaining > 0)
		{
			float boost_amount = ddm::min(ship.BoostRemaining, delta_t);
			current_velocity *= 1.0f + (ship.Acceleration * ship.BoostFactor * boost_amount);
			ship.BoostRemaining -= boost_amount;
			boosting = true;
		}
		else
		{
			ship.BoostRemaining = ddm::min(ship.BoostRemaining + (1.0f / ship.BoostRechargeRate) * delta_t, ship.BoostMaximum);
		}

		float speed = glm::length(current_velocity);
		float max_speed = boosting ? ship.MaximumSpeed * ship.BoostFactor : ship.MaximumSpeed;

		float clamped_speed = glm::clamp(speed, ship.MinimumSpeed, max_speed);
		current_velocity = glm::normalize(current_velocity) * clamped_speed;

		glm::vec3 delta_v = current_velocity * delta_t;

		ship.Velocity = current_velocity;

		transform.Position = current_position + delta_v + other_modifiers;
		transform.Update();

		// move camera after the ship
		//m_nextCameraPos = m_camera.GetPosition() + delta_v + other_modifiers;
		//m_camera.SetDirection( m_camera.GetPosition() - transform.GetWorldPosition() );
	}

	void ShipSystem::Shutdown(ddc::EntitySpace& entities)
	{
	}

	void ShipSystem::DrawDebugInternal()
	{
		if (m_lastShip.IsValid())
		{
			DD_TODO("Uncomment");
			/*ImGui::Checkbox( "Enabled", &m_enabled );

			TransformComponent* transform = m_lastShip.Get<TransformComponent>().Write();

			glm::vec3 pos = transform.GetWorldPosition();
			ImGui::Value( "Position", pos, "%.2f" );

			ShipComponent* ship = m_lastShip.Get<ShipComponent>().Write();

			glm::vec3 velocity = ship.Velocity;
			ImGui::Value( "Velocity", velocity, "%.2f" );

			ImGui::SliderFloat( "Acceleration", &ship.Acceleration, 0.0f, 100.0f, "%.2f" );

			ImGui::DragFloatRange2( "Speed", &ship.MinimumSpeed, &ship.MaximumSpeed, 1.0f, 0.0f, 100.0f, "%.2f" );

			ImGui::Value( "Current Boost", ship.BoostRemaining, "%.2f" );

			if( ImGui::TreeNodeEx( "Boost", ImGuiTreeNodeFlags_CollapsingHeader ) )
			{
				ImGui::SliderFloat( "Maximum", &ship.BoostMaximum, 0.0f, 100.0f, "%.2f" );
				ImGui::SliderFloat( "Recharge Rate", &ship.BoostRechargeRate, 0.0f, 100.0f, "%.2f" );
				ImGui::SliderFloat( "Factor", &ship.BoostFactor, 0.0f, 10.0f, "%.2f" );

				ImGui::TreePop();
			}*/
		}
	}
}