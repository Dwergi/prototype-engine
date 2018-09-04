//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "FreeCameraController.h"

#include "FPSCamera.h"
#include "InputBindings.h"

#include "glm/gtx/transform.hpp"

#include "imgui/imgui.h"

namespace dd
{
	// camera movement speed in meters per second
	const float MovementSpeed = 10.0f;
	const float BoostMultiplier = 5.0f;
	const float ZoomSpeed = 0.1f;

	// mouse sensitivity - 3840 pixels turns 180 degrees
	const float TurnSpeed = 180.f / 3840.f;

	FreeCameraController::FreeCameraController( FPSCamera& camera ) :
		m_camera( camera )
	{
		m_mouseDelta = glm::vec2( 0, 0 );

		m_inputs.Add( InputAction::FORWARD, false );
		m_inputs.Add( InputAction::BACKWARD, false );
		m_inputs.Add( InputAction::LEFT, false );
		m_inputs.Add( InputAction::RIGHT, false );
		m_inputs.Add( InputAction::UP, false );
		m_inputs.Add( InputAction::DOWN, false );
		m_inputs.Add( InputAction::BOOST, false );
	}

	FreeCameraController::FreeCameraController( FreeCameraController&& other ) : 
		m_camera( other.m_camera ),
		m_inputs( std::move( other.m_inputs ) )
	{
	}

	FreeCameraController::~FreeCameraController()
	{

	}

	void FreeCameraController::BindActions( InputBindings& bindings )
	{
		auto handle_input = [this]( InputAction action, InputType type )
		{
			HandleInput( action, type );
		};

		bindings.RegisterHandler( InputAction::FORWARD, handle_input );
		bindings.RegisterHandler( InputAction::BACKWARD, handle_input );
		bindings.RegisterHandler( InputAction::LEFT, handle_input );
		bindings.RegisterHandler( InputAction::RIGHT, handle_input );
		bindings.RegisterHandler( InputAction::UP, handle_input );
		bindings.RegisterHandler( InputAction::DOWN, handle_input );
		bindings.RegisterHandler( InputAction::BOOST, handle_input );
	}

	void FreeCameraController::HandleInput( InputAction action, InputType type )
	{
		if( !m_enabled )
			return;

		bool* state = m_inputs.Find( action );
		if( state == nullptr )
			return;

		if( type == InputType::PRESSED )
			*state = true;

		if( type == InputType::RELEASED )
			*state = false;
	}

	void FreeCameraController::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - 300, 30 ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );

		ImGui::Value( "Yaw", m_camera.GetYaw(), "%.2f" );
		ImGui::Value( "Pitch", m_camera.GetPitch(), "%.2f" );
		
		glm::vec3 position = m_camera.GetPosition();
		ImGui::Value( "Position", position, "%.1f" );

		glm::vec3 direction = m_camera.GetDirection();
		ImGui::Value( "Direction", direction, "%.2f" );

		float vfov = glm::degrees( m_camera.GetVerticalFOV() * 2.0f );
		if( ImGui::SliderFloat( "VFOV", &vfov, 0.1f, 178.0f, "%.2f" ) )
		{
			m_camera.SetVerticalFOV( glm::radians( vfov / 2.0f ) );
		}
		
		float near_distance = m_camera.GetNear();
		float far_distance = m_camera.GetFar();
		if( ImGui::SliderFloat( "Near", &near_distance, 0.01f, far_distance - 0.01f, "%.2f" ) )
		{
			m_camera.SetNear( near_distance );
		}

		if( ImGui::SliderFloat( "Far", &far_distance, near_distance + 0.01f, 5000.f, "%.2f" ) )
		{
			m_camera.SetFar( far_distance );
		}

		ImGui::Checkbox( "Invert", &m_invert );
	}
	
	void FreeCameraController::Update( float dt )
	{
		DD_PROFILE_SCOPED( FreeCameraController_Update );

		// rotate around up axis, ie. Y
		float yaw = m_camera.GetYaw();
		yaw += m_mouseDelta.x * TurnSpeed;

		float y_delta = m_mouseDelta.y * TurnSpeed;

		if( m_invert )
			y_delta = -y_delta;

		float pitch = m_camera.GetPitch();
		pitch += y_delta;

		m_camera.SetRotation( yaw, pitch );

		glm::vec3 direction = m_camera.GetDirection();

		glm::vec3 up = glm::vec3( 0, 1, 0 );

		glm::vec3 right = glm::normalize( glm::cross( direction, up ) );

		glm::vec3 movement( 0, 0, 0 );

		if( m_inputs[InputAction::FORWARD] )
			movement += direction;

		if( m_inputs[InputAction::BACKWARD] )
			movement -= direction;

		if( m_inputs[InputAction::LEFT] )
			movement -= right;

		if( m_inputs[InputAction::RIGHT] )
			movement += right;

		if( m_inputs[InputAction::UP] )
			movement += up;

		if( m_inputs[InputAction::DOWN] )
			movement -= up;

		if( glm::length( movement ) > 0 )
		{
			// normalize direction
			movement = glm::normalize( movement );

			if( m_inputs[InputAction::BOOST] )
				movement *= BoostMultiplier;

			// scale with time and speed
			glm::vec3 scaled = movement * MovementSpeed * dt;
			
			glm::vec3 position = m_camera.GetPosition();
			position += scaled;
			m_camera.SetPosition( position );
		}

		m_camera.Update( dt );
	}

	void FreeCameraController::UpdateMouse( const MousePosition& pos )
	{
		m_mouseDelta = pos.GameDelta;
	}

	void FreeCameraController::UpdateScroll( const MousePosition& pos )
	{
		float vfov = m_camera.GetVerticalFOV();
		
		float degs = glm::degrees( vfov );

		degs *= std::powf( 2.f, -pos.GameDelta.y * ZoomSpeed );

		//degs += ZoomSpeed * -pos.DeltaY;
		degs = glm::clamp( degs, 5.f, 89.f );

		m_camera.SetVerticalFOV( glm::radians( degs ) );
	}
}