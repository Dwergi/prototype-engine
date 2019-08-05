//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "IDebugPanel.h"
#include "IInputSource.h"
#include "System.h"

namespace dd
{
	struct FPSCameraComponent;
	struct InputKeyBindings;

	struct FreeCameraController : IDebugPanel, ddc::System
	{
		FreeCameraController();
		FreeCameraController( FreeCameraController&& other );
		~FreeCameraController();
		
		FreeCameraController( const FreeCameraController& ) = delete;
		FreeCameraController& operator=( const FreeCameraController& ) = delete;

		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void BindActions( InputKeyBindings& bindings ) override;

		void UpdateMouse( const MousePosition& pos );
		void UpdateScroll( const MousePosition& pos );

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Free Camera"; }

	protected:

		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;
	
	private:

		std::vector<FPSCameraComponent*> m_cameras;
		glm::vec2 m_mouseDelta { 0, 0 };
		glm::vec2 m_scrollDelta { 0, 0 };

		bool m_enabled { true };
		bool m_invert { false };
		
		DenseMap<InputAction, bool> m_inputs;

		void HandleInput( InputAction action, InputType type );
	};
}
