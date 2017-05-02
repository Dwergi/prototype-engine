//
// ShipSystem.cpp - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "PrecompiledHeader.h"
#include "ShipSystem.h"

#include "Camera.h"
#include "EntityManager.h"
#include "InputBindings.h"
#include "MeshComponent.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShipComponent.h"
#include "TransformComponent.h"

#include "imgui/imgui.h"

#include "glm/gtx/transform.hpp"

namespace dd
{
	float s_shipMesh[] = 
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

	ShipSystem::ShipSystem( Camera& camera ) :
		m_camera( camera ),
		m_enabled( false )
	{
		m_inputs.Add( InputAction::FORWARD, false );
		m_inputs.Add( InputAction::BACKWARD, false );
		m_inputs.Add( InputAction::LEFT, false );
		m_inputs.Add( InputAction::RIGHT, false );
		m_inputs.Add( InputAction::UP, false );
		m_inputs.Add( InputAction::DOWN, false );
		m_inputs.Add( InputAction::BOOST, false );
	}

	void ShipSystem::BindActions( InputBindings& bindings )
	{
		auto handle_input = std::bind( &ShipSystem::HandleInput, std::ref( *this ), std::placeholders::_1, std::placeholders::_2 );
		bindings.RegisterHandler( InputAction::FORWARD, handle_input );
		bindings.RegisterHandler( InputAction::BACKWARD, handle_input );
		bindings.RegisterHandler( InputAction::LEFT, handle_input );
		bindings.RegisterHandler( InputAction::RIGHT, handle_input );
		bindings.RegisterHandler( InputAction::UP, handle_input );
		bindings.RegisterHandler( InputAction::DOWN, handle_input );
		bindings.RegisterHandler( InputAction::BOOST, handle_input );
	}
	
	void ShipSystem::Update( EntityManager& entity_manager, float dt )
	{
		if( !m_enabled )
			return;

		entity_manager.ForAllWithReadable<TransformComponent, ShipComponent>( [this, dt]( auto entity, auto transform, auto ship )
		{
			UpdateShip( entity, transform, ship, dt );
		} );
	}

	void ShipSystem::HandleInput( InputAction action, InputType type )
	{
		bool* state = m_inputs.Find( action );
		if( state == nullptr )
			return;

		if( type == InputType::PRESSED )
			*state = true;

		if( type == InputType::RELEASED )
			*state = false;
	}

	namespace
	{
		ShaderHandle CreateShaders( const char* name )
		{
			Vector<Shader> shaders;

			Shader vert = Shader::Create( String8( "vertex" ), String8( "" ), Shader::Type::Vertex );
			DD_ASSERT( vert.IsValid() );
			shaders.Add( vert );

			Shader pixel = Shader::Create( String8( "pixel" ), String8( "" ), Shader::Type::Pixel );
			DD_ASSERT( pixel.IsValid() );
			shaders.Add( pixel );

			ShaderHandle handle = ShaderProgram::Create( String8( name ), shaders );
			return handle;
		}
	}

	void ShipSystem::CreateShip( EntityManager& entity_manager )
	{
		EntityHandle entity = entity_manager.CreateEntity<TransformComponent, MeshComponent, ShipComponent>();

		glm::mat4 transform = glm::translate( glm::vec3( 0, 2.5, 10 ) );

		TransformComponent* transform_cmp = entity_manager.GetWritable<TransformComponent>( entity );
		transform_cmp->SetLocalTransform( transform );

		ShaderHandle shader = CreateShaders( "ship" );
		/*shader.Get()->Use( true );
		shader.Get()->BindAttributeFloat( "Position", 3, 6, 0, false );
		shader.Get()->BindAttributeFloat( "Normal", 3, 6, 3, false );
		shader.Get()->Use( false );*/

		AABB bounds;
		bounds.Expand( glm::vec3( 0, 0, 1 ) );
		bounds.Expand( glm::vec3( 0.5f, 0, -1.0 ) );
		bounds.Expand( glm::vec3( -0.5f, 0, -1.0 ) );

		MeshHandle mesh_h = Mesh::Create( "ship", shader );
		mesh_h.Get()->MakeUnitCube();
		/*mesh_h.Get()->SetData( s_shipMesh, sizeof( s_shipMesh ), 6 );
		mesh_h.Get()->SetBounds( bounds );*/

		MeshComponent* mesh_cmp = entity_manager.GetWritable<MeshComponent>( entity );
		mesh_cmp->Mesh = mesh_h;
		mesh_cmp->Colour = glm::vec4( 1, 0, 0, 1 );
		mesh_cmp->Hidden = false;
		mesh_cmp->UpdateBounds( transform );

		ShipComponent* ship_cmp = entity_manager.GetWritable<ShipComponent>( entity );
		ship_cmp->Acceleration = 5.0f;
		ship_cmp->Velocity = glm::vec3( 0, 0, 1 );
		ship_cmp->BoostFactor = 2.0f;
		ship_cmp->MaximumSpeed = 20.0f;
		ship_cmp->MinimumSpeed = 5.0f;
		ship_cmp->BoostMaximum = 2.5f;
		ship_cmp->BoostRemaining = ship_cmp->BoostMaximum;
		ship_cmp->BoostRechargeRate = 2.0f;
	}

	void ShipSystem::UpdateShip( EntityHandle entity, ComponentHandle<TransformComponent> transform, ComponentHandle<ShipComponent> ship, float delta_t )
	{
		const TransformComponent* transform_read = transform.Read();
		TransformComponent* transform_write = transform.Write();

		const ShipComponent* ship_read = ship.Read();
		ShipComponent* ship_write = ship.Write();

		if( transform_read == nullptr ||
			transform_write == nullptr ||
			ship_read == nullptr ||
			ship_write == nullptr )
		{
			return;
		}

		glm::vec3 current_velocity = ship_read->Velocity;
		float current_speed = glm::length( current_velocity );
		glm::vec3 current_position = transform_read->GetLocalPosition();
		bool boosting = false;

		glm::vec3 other_modifiers = glm::vec3( 0, 0, 0 );

		glm::vec3 up = glm::vec3( 0, 1, 0 );
		glm::vec3 right = glm::normalize( glm::cross( current_velocity, up ) );

		if( m_inputs[InputAction::FORWARD] )
			current_velocity *= 1.0f + (ship_read->Acceleration * delta_t);

		if( m_inputs[InputAction::BACKWARD] )
			current_velocity *= 1.0f - (ship_read->Acceleration * delta_t);

		if( m_inputs[InputAction::UP] )
			other_modifiers += up * current_speed * delta_t;

		if( m_inputs[InputAction::DOWN] )
			other_modifiers -= up * current_speed * delta_t;

		if( m_inputs[InputAction::RIGHT] )
			other_modifiers += right * current_speed * delta_t;

		if( m_inputs[InputAction::LEFT] )
			other_modifiers -= right * current_speed * delta_t;

		if( m_inputs[InputAction::BOOST] && ship_read->BoostRemaining > 0 )
		{
			float boost_amount = std::min( ship_write->BoostRemaining, delta_t );
			current_velocity *= 1.0f + (ship_read->Acceleration * ship_read->BoostFactor * boost_amount);
			ship_write->BoostRemaining -= boost_amount;
			boosting = true;
		}
		else
		{
			ship_write->BoostRemaining = std::min( ship_read->BoostRemaining + (1.0f / ship_read->BoostRechargeRate) * delta_t, ship_read->BoostMaximum );
		}

		float speed = glm::length( current_velocity );
		float max_speed = boosting ? ship_read->MaximumSpeed * ship_read->BoostFactor : ship_read->MaximumSpeed;

		float clamped_speed = glm::clamp( speed, ship_read->MinimumSpeed, max_speed );
		current_velocity = glm::normalize( current_velocity ) * clamped_speed;

		glm::vec3 delta_v = current_velocity * delta_t;

		ship_write->Velocity = current_velocity;

		transform_write->SetLocalPosition( current_position + delta_v + other_modifiers );

		// move camera after the ship
		m_nextCameraPos = m_camera.GetPosition() + delta_v + other_modifiers;
		//m_camera.SetDirection( m_camera.GetPosition() - transform_write->GetWorldPosition() );

		m_lastShip = entity;
	}

	void ShipSystem::PostRender( EntityManager& entity_manager, float dt )
	{
		if( !m_enabled )
			return;

		m_camera.SetPosition( m_nextCameraPos );
	}

	void ShipSystem::DrawDebugInternal()
	{
		if( ImGui::Checkbox( "Enabled", &m_enabled ) )
			m_enabled = false;

		TransformComponent* transform = m_lastShip.Get<TransformComponent>().Write();

		glm::vec3 pos = transform->GetWorldPosition();
		ImGui::Text( "Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z );

		ShipComponent* ship = m_lastShip.Get<ShipComponent>().Write();

		glm::vec3 velocity = ship->Velocity;
		ImGui::Text( "Velocity: %.2f %.2f %.2f", velocity.x, velocity.y, velocity.z );

		ImGui::SliderFloat( "Acceleration", &ship->Acceleration, 0.0f, 100.0f, "%.2f" );

		ImGui::DragFloatRange2( "Speed", &ship->MinimumSpeed, &ship->MaximumSpeed, 1.0f, 0.0f, 100.0f, "%.2f" );

		ImGui::Text( "Current Boost: %.2f", ship->BoostRemaining );

		if( ImGui::TreeNodeEx( "Boost", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::SliderFloat( "Maximum", &ship->BoostMaximum, 0.0f, 100.0f, "%.2f" );
			ImGui::SliderFloat( "Recharge Rate", &ship->BoostRechargeRate, 0.0f, 100.0f, "%.2f" );
			ImGui::SliderFloat( "Factor", &ship->BoostFactor, 0.0f, 10.0f, "%.2f" );

			ImGui::TreePop();
		}
	}
}