//
// Camera.h - A generic perspective camera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include "ICamera.h"

#include <glm/glm.hpp>

namespace dd
{
	class Window;

	class FPSCamera : public ICamera
	{
	public:

		FPSCamera();
		FPSCamera( Window& window );

		void SetRotation( float yaw, float pitch );

		float GetYaw() const { return m_yaw; }
		float GetPitch() const { return m_pitch; }

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

		virtual bool IsDirty() const override { return m_dirty; }
		void SetClean() { m_dirty = false; }

		void CopyValuesFrom( const FPSCamera& other );
		
		BEGIN_SCRIPT_OBJECT( FPSCamera )
			METHOD( FPSCamera, SetRotation )
			METHOD( FPSCamera, GetYaw )
			METHOD( FPSCamera, GetPitch )
			METHOD( FPSCamera, GetPosition )
			METHOD( FPSCamera, SetPosition )
			METHOD( FPSCamera, GetDirection )
			METHOD( FPSCamera, SetDirection )
			METHOD( FPSCamera, GetNear )
			METHOD( FPSCamera, SetNear )
			METHOD( FPSCamera, GetFar )
			METHOD( FPSCamera, SetFar )
			METHOD( FPSCamera, GetAspectRatio )
			METHOD( FPSCamera, SetAspectRatio )
			METHOD( FPSCamera, GetProjectionMatrix )
			METHOD( FPSCamera, GetVerticalFOV )
			METHOD( FPSCamera, SetVerticalFOV )
		END_TYPE

	private:

		glm::vec3 m_direction;
		glm::vec3 m_position;

		// half vertical FOV in radians
		float m_vfov { 0 };
		float m_near { 0 };
		float m_far { 0 };
		float m_aspectRatio { 0 };
		float m_yaw { 0 };
		float m_pitch { 0 };
		bool m_dirty { true };
	};
}
