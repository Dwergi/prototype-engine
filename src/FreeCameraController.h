//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "Camera.h"
#include "Input.h"

namespace dd
{
	class FreeCameraController
	{
	public:

		FreeCameraController( Camera& camera );
		FreeCameraController( FreeCameraController&& other );
		~FreeCameraController();
		
		FreeCameraController( const FreeCameraController& ) = delete;
		FreeCameraController& operator=( const FreeCameraController& ) = delete;

		void UpdateMouse( const MousePosition& pos );
		void HandleInput( InputAction action, InputType type );
		void Update( float dt );

		void DrawCameraDebug() const;

	private:

		Camera& m_camera;
		float m_yaw;
		float m_pitch;
		glm::vec2 m_mouseDelta;
		glm::vec3 m_position;
		glm::vec3 m_direction;

		DenseMap<InputAction, bool> m_inputs;
	};
}
