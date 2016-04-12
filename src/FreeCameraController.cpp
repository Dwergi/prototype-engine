//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "FreeCameraController.h"

#include <glm/gtx/transform.hpp>

namespace dd
{
	float wrap( float value, float min, float max )
	{
		value = min + std::fmod( value - min, max - min );
		return value;
	}

	// camera movement speed in meters per second
	const float MovementSpeed = 10.0f;
	const float BoostMultiplier = 5.0f;

	// mouse sensitivity - 1920 pixels turns 180 degrees
	const float TurnSpeed = 180.f / 1920.f;

	FreeCameraController::FreeCameraController( Camera& camera ) :
		m_camera( camera ),
		m_yaw( 0.0f ),
		m_pitch( 0.0f )
	{
		m_mouseDelta = glm::vec2( 0, 0 );
		m_position = glm::vec3( 0, 0, 0 );
		m_up = glm::vec3( 0, 1, 0 );

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

	void FreeCameraController::Update( float dt )
	{
		// rotate around up axis, ie. Y
		m_yaw += m_mouseDelta.x * TurnSpeed;
		m_pitch += -m_mouseDelta.y * TurnSpeed;

		// wrap the x direction
		m_yaw = wrap( m_yaw, 0, 360 );

		// clamp the y direction
		m_pitch = glm::clamp( m_pitch, -89.f, 89.f );

		glm::vec3 direction;
		direction.x = cos( glm::radians( m_pitch ) ) * cos( glm::radians( m_yaw ) );
		direction.y = sin( glm::radians( m_pitch ) );
		direction.z = cos( glm::radians( m_pitch ) ) * sin( glm::radians( m_yaw ) );
		direction = glm::normalize( direction );

		glm::vec3 right = glm::normalize( glm::cross( direction, m_up ) );

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
			movement += m_up;

		if( m_inputs[InputAction::DOWN] )
			movement -= m_up;

		if( m_inputs[InputAction::BOOST] )
			movement *= BoostMultiplier;

		if( glm::length( movement ) > 0 )
		{
			movement = glm::normalize( movement );

			// scale with time and speed
			glm::vec3 scaled = movement * MovementSpeed * dt;
			
			m_position += scaled;
		}

		glm::mat4 lookAt = glm::lookAt( m_position, direction, m_up );
		m_camera.SetTransform( lookAt );
	}

	void FreeCameraController::UpdateMouse( const MousePosition& pos )
	{
		m_mouseDelta.x = pos.DeltaX;
		m_mouseDelta.y = pos.DeltaY;
	}
}