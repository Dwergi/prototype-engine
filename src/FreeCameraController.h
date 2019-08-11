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

		virtual void Initialize(ddc::EntitySpace& space) override;
		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Free Camera"; }

	protected:

		virtual void DrawDebugInternal() override;
	
	private:

		std::vector<FPSCameraComponent*> m_cameras;
		glm::vec2 m_mouseDelta { 0, 0 };

		bool m_enabled { true };
		bool m_invert { false };
	};
}
