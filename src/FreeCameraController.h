//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "IDebugPanel.h"
#include "IInputSource.h"

#include "ddc/System.h"

namespace dd
{
	struct FPSCameraComponent;
	struct InputKeyBindings;

	struct FreeCameraController : IDebugPanel, ddc::System
	{
		FreeCameraController();
		FreeCameraController(FreeCameraController&& other);
		~FreeCameraController();

		FreeCameraController(const FreeCameraController&) = delete;
		FreeCameraController& operator=(const FreeCameraController&) = delete;

		virtual void Initialize(ddc::EntityLayer& layer) override;
		virtual void Update(ddc::UpdateData& data) override;

	private:

		std::vector<FPSCameraComponent*> m_cameras;
		glm::vec2 m_mouseDelta { 0, 0 };

		bool m_enabled { true };
		bool m_invert { false };

		virtual const char* GetDebugTitle() const override { return "Free Camera"; }
		virtual void DrawDebugInternal() override;
	};
}
