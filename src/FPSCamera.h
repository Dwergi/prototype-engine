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
		virtual glm::mat4 GetCameraMatrix() const override;

		virtual const ddr::Frustum& GetFrustum() const override { return m_frustum; }

		virtual void Update( float delta_t ) override;

		void CopyValuesFrom( const FPSCamera& other );
		
		DD_SCRIPT_OBJECT( FPSCamera )
			DD_METHOD( FPSCamera, SetRotation )
			DD_METHOD( FPSCamera, GetYaw )
			DD_METHOD( FPSCamera, GetPitch )
			DD_METHOD( FPSCamera, GetPosition )
			DD_METHOD( FPSCamera, SetPosition )
			DD_METHOD( FPSCamera, GetDirection )
			DD_METHOD( FPSCamera, SetDirection )
			DD_METHOD( FPSCamera, GetNear )
			DD_METHOD( FPSCamera, SetNear )
			DD_METHOD( FPSCamera, GetFar )
			DD_METHOD( FPSCamera, SetFar )
			DD_METHOD( FPSCamera, GetAspectRatio )
			DD_METHOD( FPSCamera, SetAspectRatio )
			DD_METHOD( FPSCamera, GetProjectionMatrix )
			DD_METHOD( FPSCamera, GetVerticalFOV )
			DD_METHOD( FPSCamera, SetVerticalFOV )
		DD_END_TYPE

	private:

		ddr::Frustum m_frustum;

		glm::vec3 m_direction;
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
