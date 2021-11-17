//
// Camera.h - A generic perspective camera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "ICamera.h"
#include "Frustum.h"

#include <glm/glm.hpp>

namespace dd
{
	struct Window;

	struct FPSCameraComponent : ddr::ICamera
	{
		void SetRotation(float yaw, float pitch);

		virtual float GetYaw() const override { return m_yaw; }
		virtual float GetPitch() const override { return m_pitch; }

		virtual glm::vec3 GetPosition() const override;
		void SetPosition(const glm::vec3& pos);

		virtual glm::vec3 GetDirection() const override;
		void SetDirection(const glm::vec3& dir);

		virtual float GetNear() const override;
		void SetNear(float dist_near);

		virtual float GetFar() const override;
		void SetFar(float dist_far);

		virtual float GetAspectRatio() const override;
		void SetAspectRatio(float ratio);

		virtual float GetVerticalFOV() const override;
		void SetVerticalFOV(float vfov);

		virtual glm::mat4 GetProjectionMatrix() const override;
		virtual glm::mat4 GetViewMatrix() const override;

		virtual const ddr::Frustum& GetFrustum() const override { return m_frustum; }

		virtual void Update(float delta_t) override;

		void CopyValuesFrom(const FPSCameraComponent& other);

		DD_BEGIN_CLASS(dd::FPSCameraComponent)
			DD_COMPONENT();

			DD_METHOD(SetRotation);
			DD_METHOD(GetYaw);
			DD_METHOD(GetPitch);
			DD_METHOD(GetPosition);
			DD_METHOD(SetPosition);
			DD_METHOD(GetDirection);
			DD_METHOD(SetDirection);
			DD_METHOD(GetNear);
			DD_METHOD(SetNear);
			DD_METHOD(GetFar);
			DD_METHOD(SetFar);
			DD_METHOD(GetAspectRatio);
			DD_METHOD(SetAspectRatio);
			DD_METHOD(GetProjectionMatrix);
			DD_METHOD(GetVerticalFOV);
			DD_METHOD(SetVerticalFOV);
		DD_END_CLASS()

	private:

		ddr::Frustum m_frustum;

		glm::vec3 m_position;

		// half vertical FOV in radians
		float m_vfov { 0 };
		float m_near { 0.01 };
		float m_far { 5000 };
		float m_aspectRatio { 0 };
		float m_yaw { 0 };
		float m_pitch { 0 };
	};
}
