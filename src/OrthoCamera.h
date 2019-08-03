#pragma once

#include "ICamera.h"

namespace ddr
{
	struct OrthoCamera : ICamera
	{
		// Inherited via ICamera
		virtual glm::mat4 GetProjectionMatrix() const override;
		virtual glm::mat4 GetViewMatrix() const override;
		virtual float GetVerticalFOV() const override;
		virtual float GetAspectRatio() const override;
		virtual float GetNear() const override;
		virtual float GetFar() const override;
		virtual glm::vec3 GetDirection() const override;
		virtual glm::vec3 GetPosition() const override;
		virtual float GetYaw() const override;
		virtual float GetPitch() const override;
		virtual const ddr::Frustum& GetFrustum() const override;
		virtual void Update(float delta_t) override;
	};
}