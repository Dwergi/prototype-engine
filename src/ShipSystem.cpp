//
// ShipSystem.cpp - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "PCH.h"
#include "ShipSystem.h"

#include "FPSCameraComponent.h"
#include "InputKeyBindings.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "ShipComponent.h"
#include "TransformComponent.h"

namespace dd
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

	ShipSystem::ShipSystem() :
		ddc::System( "Ship System" )
	{
		m_inputs.Add( InputAction::FORWARD,		false );
		m_inputs.Add( InputAction::BACKWARD,	false );
		m_inputs.Add( InputAction::LEFT,		false );
		m_inputs.Add( InputAction::RIGHT,		false );
		m_inputs.Add( InputAction::UP,			false );
		m_inputs.Add( InputAction::DOWN,		false );
		m_inputs.Add( InputAction::BOOST,		false );
	}

	void ShipSystem::BindActions( InputKeyBindings& bindings )
	{
		auto handle_input = [this]( InputAction action, InputType type )
		{
			HandleInput( action, type );
		};

		bindings.RegisterHandler( InputAction::FORWARD,		handle_input );
		bindings.RegisterHandler( InputAction::BACKWARD,	handle_input );
		bindings.RegisterHandler( InputAction::LEFT,		handle_input );
		bindings.RegisterHandler( InputAction::RIGHT,		handle_input );
		bindings.RegisterHandler( InputAction::UP,			handle_input );
		bindings.RegisterHandler( InputAction::DOWN,		handle_input );
		bindings.RegisterHandler( InputAction::BOOST,		handle_input );
	}
	
	void ShipSystem::Update( const ddc::UpdateData& data )
	{
		if( !m_enabled )
			return;

		DD_TODO( "Uncomment" );

	/*	entities.ForAllWithReadable<TransformComponent, ShipComponent>( [this, dt]( auto entity, auto transform, auto ship )
		{
			UpdateShip( entity, transform, ship, dt );
		} );*/
	}

	void ShipSystem::HandleInput( InputAction action, InputType type )
	{
		bool* state = m_inputs.Find( action );
		if( state == nullptr )
			return;

		if( type == InputType::Press )
			*state = true;

		if( type == InputType::Release )
			*state = false;
	}

	void ShipSystem::CreateShip( ddc::EntitySpace& entities )
	{
		DD_TODO( "Uncomment" );
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

	void ShipSystem::UpdateShip( TransformComponent& transform, ShipComponent& ship, float delta_t )
	{
		glm::vec3 current_velocity = ship.Velocity;
		float current_speed = glm::length( current_velocity );
		glm::vec3 current_position = transform.Position;
		bool boosting = false;

		glm::vec3 other_modifiers = glm::vec3( 0, 0, 0 );

		glm::vec3 up = glm::vec3( 0, 1, 0 );
		glm::vec3 right = glm::normalize( glm::cross( current_velocity, up ) );

		if( m_inputs[InputAction::FORWARD] )
			current_velocity *= 1.0f + (ship.Acceleration * delta_t);

		if( m_inputs[InputAction::BACKWARD] )
			current_velocity *= 1.0f - (ship.Acceleration * delta_t);

		if( m_inputs[InputAction::UP] )
			other_modifiers += up * current_speed * delta_t;

		if( m_inputs[InputAction::DOWN] )
			other_modifiers -= up * current_speed * delta_t;

		if( m_inputs[InputAction::RIGHT] )
			other_modifiers += right * current_speed * delta_t;

		if( m_inputs[InputAction::LEFT] )
			other_modifiers -= right * current_speed * delta_t;

		if( m_inputs[InputAction::BOOST] && ship.BoostRemaining > 0 )
		{
			float boost_amount = ddm::min( ship.BoostRemaining, delta_t );
			current_velocity *= 1.0f + (ship.Acceleration * ship.BoostFactor * boost_amount);
			ship.BoostRemaining -= boost_amount;
			boosting = true;
		}
		else
		{
			ship.BoostRemaining = ddm::min( ship.BoostRemaining + (1.0f / ship.BoostRechargeRate) * delta_t, ship.BoostMaximum );
		}

		float speed = glm::length( current_velocity );
		float max_speed = boosting ? ship.MaximumSpeed * ship.BoostFactor : ship.MaximumSpeed;

		float clamped_speed = glm::clamp( speed, ship.MinimumSpeed, max_speed );
		current_velocity = glm::normalize( current_velocity ) * clamped_speed;

		glm::vec3 delta_v = current_velocity * delta_t;

		ship.Velocity = current_velocity;

		transform.Position = current_position + delta_v + other_modifiers;
		transform.Update();

		// move camera after the ship
		//m_nextCameraPos = m_camera.GetPosition() + delta_v + other_modifiers;
		//m_camera.SetDirection( m_camera.GetPosition() - transform.GetWorldPosition() );
	}

	void ShipSystem::Shutdown( ddc::EntitySpace& entities )
	{
	}

	void ShipSystem::DrawDebugInternal( ddc::EntitySpace& entities )
	{
		if( m_lastShip.IsValid() )
		{
			DD_TODO( "Uncomment" );
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