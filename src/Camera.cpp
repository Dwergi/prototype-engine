//
// Camera.h - A generic perspective camera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "Camera.h"

#include "Window.h"

#include <glm/gtc/matrix_transform.hpp>

namespace dd
{
	Camera::Camera() :
		m_far( 0.0f ),
		m_near( 0.0f ),
		m_aspectRatio( 0.0f ),
		m_vfov( 0.0f )
	{
	}

	Camera::Camera( Window& window ) :
		m_far( 1000.f ),
		m_near( 0.1f )
	{
		m_aspectRatio = (float) window.GetWidth() / (float) window.GetHeight();
		m_vfov = glm::radians( 45.f );
	}

	glm::vec3 Camera::GetPosition() const
	{
		return m_position;
	}

	void Camera::SetPosition( const glm::vec3& pos )
	{
		m_position = pos;
	}

	glm::vec3 Camera::GetDirection() const
	{
		return m_direction;
	}
	
	void Camera::SetDirection( const glm::vec3& dir )
	{
		m_direction = glm::normalize( dir );
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

	float Camera::GetVerticalFOV() const
	{
		return m_vfov;
	}

	void Camera::SetVerticalFOV( float vfov )
	{
		m_vfov = vfov;
	}

	float Camera::GetHorizontalFOV() const
	{
		return m_vfov * m_aspectRatio;
	}

	void Camera::SetHorizontalFOV( float hfov )
	{
		m_vfov = hfov / m_aspectRatio;
	}

	float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	void Camera::SetAspectRatio( int w, int h )
	{
		m_aspectRatio = float( w ) / float( h );
	}

	glm::mat4 Camera::GetProjection() const
	{
		return glm::perspective( m_vfov, m_aspectRatio, m_near, m_far );
	}

	glm::mat4 Camera::GetCameraMatrix() const
	{
		return glm::lookAt( m_position, m_position + m_direction, glm::vec3( 0, 1, 0 ) );
	}

	void Camera::CopyValuesFrom( const Camera& other )
	{
		m_vfov = other.m_vfov;
		m_aspectRatio = other.m_aspectRatio;
		m_near = other.m_near;
		m_far = other.m_far;
		m_direction = other.m_direction;
		m_position = other.m_position;
	}
}