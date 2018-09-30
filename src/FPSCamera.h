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
	class Window;

	class FPSCamera : public ddr::ICamera
	{
	public:

		FPSCamera();
		FPSCamera( Window& window );

		void SetRotation( float yaw, float pitch );

		virtual float GetYaw() const override { return m_yaw; }
		virtual float GetPitch() const override { return m_pitch; }

		virtual glm::vec3 GetPosition() const override;
		void SetPosition( const glm::vec3& pos );

		virtual glm::vec3 GetDirection() const override;
		void SetDirection( const glm::vec3& dir );

		virtual float GetNear() const override;
		void SetNear( float dist_near );

		virtual float GetFar() const override;
		void SetFar( float dist_far );

		virtual float GetAspectRatio() const override;
		void SetAspectRatio( int w, int h );

		virtual float GetVerticalFOV() const override;
		void SetVerticalFOV( float vfov );

		virtual glm::mat4 GetProjectionMatrix() const override;
		virtual glm::mat4 GetViewMatrix() const override;

		virtual const ddr::Frustum& GetFrustum() const override { return m_frustum; }

		virtual void Update( float delta_t ) override;

		void CopyValuesFrom( const FPSCamera& other );
		
		DD_CLASS( dd::FPSCamera )
		{
			DD_METHOD( dd::FPSCamera, SetRotation );
			DD_METHOD( dd::FPSCamera, GetYaw );
			DD_METHOD( dd::FPSCamera, GetPitch );
			DD_METHOD( dd::FPSCamera, GetPosition );
			DD_METHOD( dd::FPSCamera, SetPosition );
			DD_METHOD( dd::FPSCamera, GetDirection );
			DD_METHOD( dd::FPSCamera, SetDirection );
			DD_METHOD( dd::FPSCamera, GetNear );
			DD_METHOD( dd::FPSCamera, SetNear );
			DD_METHOD( dd::FPSCamera, GetFar );
			DD_METHOD( dd::FPSCamera, SetFar );
			DD_METHOD( dd::FPSCamera, GetAspectRatio );
			DD_METHOD( dd::FPSCamera, SetAspectRatio );
			DD_METHOD( dd::FPSCamera, GetProjectionMatrix );
			DD_METHOD( dd::FPSCamera, GetVerticalFOV );
			DD_METHOD( dd::FPSCamera, SetVerticalFOV );
		}

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
