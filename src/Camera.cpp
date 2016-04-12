//
// Camera.h - A generic perspective camera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace dd
{
	Camera::Camera() :
		m_vfov( (float) M_PI_2 ), // 90 degrees in radians
		m_far( 1000.f ),
		m_near( 0.001f ),
		m_aspectRatio( 16.f / 9.f )
	{
		
	}

	glm::vec3 Camera::GetPosition() const
	{
		return m_transform[3].xyz();
	}

	glm::mat4 Camera::GetTransform() const
	{
		return m_transform;
	}

	void Camera::SetPosition( const glm::vec3& pos )
	{
		m_transform[3] = glm::vec4( pos, 0 );
	}
	
	void Camera::SetTransform( const glm::mat4& transform )
	{
		m_transform = transform;
	}

	float Camera::GetNear() const
	{
		return m_near;
	}

	void Camera::SetNear( float dist_near )
	{
		m_near = dist_near;
	}

	float Camera::GetFar() const
	{
		return m_far;
	}

	void Camera::SetFar( float dist_far )
	{
		m_far = dist_far;
	}

	float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	void Camera::SetAspectRatio( int w, int h )
	{
		m_aspectRatio = float( w ) / float( h );
	}

	glm::mat4 Camera::GetProjection()
	{
		return glm::perspective( m_vfov, m_aspectRatio, m_near, m_far );
	}
}