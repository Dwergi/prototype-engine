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
	class InputBindings;

	class FreeCameraController
	{
	public:

		FreeCameraController( Camera& camera );
		FreeCameraController( FreeCameraController&& other );
		~FreeCameraController();
		
		FreeCameraController( const FreeCameraController& ) = delete;
		FreeCameraController& operator=( const FreeCameraController& ) = delete;

		void Update( float dt );

		void UpdateMouse( const MousePosition& pos );
		void UpdateScroll( const MousePosition& pos );
		void HandleInput( InputAction action, InputType type );

		void BindActions( InputBindings& bindings );

		void DrawCameraDebug() const;

	private:

		Camera& m_camera;
		float m_yaw;
		float m_pitch;
		glm::vec2 m_mouseDelta;

		DenseMap<InputAction, bool> m_inputs;
	};
}
