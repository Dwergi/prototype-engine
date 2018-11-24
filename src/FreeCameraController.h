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
	struct InputBindings;

	struct FreeCameraController : IDebugPanel, ddc::System
	{
		FreeCameraController();
		FreeCameraController( FreeCameraController&& other );
		~FreeCameraController();
		
		FreeCameraController( const FreeCameraController& ) = delete;
		FreeCameraController& operator=( const FreeCameraController& ) = delete;

		virtual void Update( const ddc::UpdateData& data ) override;

		void UpdateMouse( const MousePosition& pos );
		void UpdateScroll( const MousePosition& pos );
		void HandleInput( InputAction action, InputType type );
		void BindActions( InputBindings& bindings );

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Free Camera"; }

	protected:

		virtual void DrawDebugInternal( ddc::World& world ) override;
	
	private:

		std::vector<FPSCameraComponent*> m_cameras;
		glm::vec2 m_mouseDelta;
		glm::vec2 m_scrollDelta;

		bool m_enabled { true };
		bool m_invert { false };
		
		DenseMap<InputAction, bool> m_inputs;
	};
}
