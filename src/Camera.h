//
// Camera.h - A generic perspective camera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#pragma once

#include <glm/glm.hpp>

namespace dd
{
	class Window;

	class Camera
	{
	public:

		Camera();
		Camera( Window& window );

		glm::vec3 GetPosition() const;
		void SetPosition( const glm::vec3& pos );

		glm::mat4 GetTransform() const;
		void SetTransform( const glm::mat4& transform );

		float GetNear() const;
		void SetNear( float dist_near );

		float GetFar() const;
		void SetFar( float dist_far );

		float GetAspectRatio() const;
		void SetAspectRatio( int w, int h );

		float GetVerticalFOV() const;
		void SetVerticalFOV( float vfov );

		glm::mat4 GetProjection() const;
		
		BEGIN_SCRIPT_OBJECT( Camera )
			METHOD( Camera, GetPosition )
			METHOD( Camera, SetPosition )
			METHOD( Camera, GetTransform )
			METHOD( Camera, SetTransform )
			METHOD( Camera, GetNear )
			METHOD( Camera, SetNear )
			METHOD( Camera, GetFar )
			METHOD( Camera, SetFar )
			METHOD( Camera, GetAspectRatio )
			METHOD( Camera, SetAspectRatio )
			METHOD( Camera, GetProjection )
			METHOD( Camera, GetVerticalFOV )
			METHOD( Camera, SetVerticalFOV )
		END_TYPE

	private:

		glm::mat4 m_transform;
		float m_vfov;
		float m_near;
		float m_far;
		float m_aspectRatio;
	};
}
