//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "FreeCameraController.h"

#include "glm/gtx/transform.hpp"

#include "imgui/imgui.h"

namespace dd
{
	float wrap( float value, float min, float max )
	{
		value = min + std::fmod( value - min, max - min );

		if( value < 0 )
			value += max;

		return value;
	}

	// camera movement speed in meters per second
	const float MovementSpeed = 10.0f;
	const float BoostMultiplier = 5.0f;
	const float ZoomSpeed = 0.1f;

	// mouse sensitivity - 3840 pixels turns 180 degrees
	const float TurnSpeed = 180.f / 3840.f;

	FreeCameraController::FreeCameraController( Camera& camera ) :
		m_camera( camera ),
		m_yaw( 0.0f ),
		m_pitch( 0.0f )
	{
		m_mouseDelta = glm::vec2( 0, 0 );
		m_position = glm::vec3( 5, 0, 0 );

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

	void FreeCameraController::HandleInput( InputAction action, InputType type )
	{
		bool* state = m_inputs.Find( action );
		if( state == nullptr )
			return;

		if( type == InputType::PRESSED )
			*state = true;

		if( type == InputType::RELEASED )
			*state = false;
	}

	void FreeCameraController::DrawCameraDebug() const
	{
		bool open = true;
		if( !ImGui::Begin( "FreeCamera", &open, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImGui::End();
			return;
		}

		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x - 2, 2 ) );

		ImGui::Text( "Yaw: %.1f", m_yaw );
		ImGui::Text( "Pitch: %.1f", m_pitch );
		ImGui::Text( "Position: %.1f, %.1f, %.1f", m_position.x, m_position.y, m_position.z );
		ImGui::Text( "Direction: %.1f, %.1f, %.1f", m_direction.x, m_direction.y, m_direction.z );
		ImGui::Text( "VFOV: %.1f", glm::degrees( m_camera.GetVerticalFOV() ) * 2.f );
		ImGui::End();
	}

	void FreeCameraController::Update( float dt )
	{
		DD_PROFILE_SCOPED( FreeCameraController_Update );

		// rotate around up axis, ie. Y
		m_yaw += m_mouseDelta.x * TurnSpeed;
		m_pitch += m_mouseDelta.y * TurnSpeed;

		// wrap the x direction
		m_yaw = wrap( m_yaw, 0, 360 );

		// clamp the y direction
		m_pitch = glm::clamp( m_pitch, -89.f, 89.f );

		glm::vec3 direction( std::cos( glm::radians( m_pitch ) ) * std::sin( glm::radians( m_yaw ) ),
							 std::sin( glm::radians( m_pitch ) ),
							 std::cos( glm::radians( m_pitch ) ) * std::cos( glm::radians( m_yaw ) ) );

		direction = glm::normalize( direction );

		glm::vec3 up = glm::vec3( 0, 1, 0 );

		glm::vec3 right = glm::normalize( glm::cross( direction.xyz(), up ) );

		m_direction = direction;

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
			
			m_position += scaled;
		}

		glm::mat4 lookAt = glm::lookAt( m_position, m_position + m_direction, up );
		m_camera.SetTransform( lookAt );
	}

	void FreeCameraController::UpdateMouse( const MousePosition& pos )
	{
		m_mouseDelta.x = pos.DeltaX;
		m_mouseDelta.y = pos.DeltaY;
	}

	void FreeCameraController::UpdateScroll( const MousePosition& pos )
	{
		float vfov = m_camera.GetVerticalFOV();
		
		float degs = glm::degrees( vfov );

		degs *= std::powf( 2.f, -pos.DeltaY * ZoomSpeed );

		//degs += ZoomSpeed * -pos.DeltaY;
		degs = glm::clamp( degs, 5.f, 89.f );

		m_camera.SetVerticalFOV( glm::radians( degs ) );
	}
}