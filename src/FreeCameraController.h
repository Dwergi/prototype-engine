//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "IDebugPanel.h"
#include "Input.h"

namespace dd
{
	class FPSCamera;
	class InputBindings;

	class FreeCameraController : public IDebugPanel
	{
	public:

		FreeCameraController( FPSCamera& camera );
		FreeCameraController( FreeCameraController&& other );
		~FreeCameraController();
		
		FreeCameraController( const FreeCameraController& ) = delete;
		FreeCameraController& operator=( const FreeCameraController& ) = delete;

		void Update( float dt );

		void UpdateMouse( const MousePosition& pos );
		void UpdateScroll( const MousePosition& pos );
		void HandleInput( InputAction action, InputType type );
		void BindActions( InputBindings& bindings );

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Free Camera"; }

	protected:

		virtual void DrawDebugInternal() override;
	
	private:

		FPSCamera & m_camera;
		glm::vec2 m_mouseDelta;

		bool m_enabled { true };
		bool m_invert { false };
		
		DenseMap<InputAction, bool> m_inputs;
	};
}
