//
// FPSCamera.h - A generic perspective FPSCamera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PrecompiledHeader.h"
#include "FPSCamera.h"

#include "Math_dd.h"
#include "Window.h"

#include <glm/gtc/matrix_transform.hpp>

namespace dd
{
	FPSCamera::FPSCamera() :
		m_far( 0.0f ),
		m_near( 0.0f ),
		m_aspectRatio( 0.0f ),
		m_vfov( 0.0f )
	{
	}

	FPSCamera::FPSCamera( Window& window )
	{
		m_aspectRatio = (float) window.GetWidth() / (float) window.GetHeight();
		m_vfov = glm::radians( 45.f );
	}

	glm::vec3 FPSCamera::GetPosition() const
	{
		return m_position;
	}

	void FPSCamera::SetPosition( const glm::vec3& pos )
	{
		if( m_position == pos )
		{
			return;
		}

		m_dirty = true;

		m_position = pos;
	}

	void FPSCamera::SetRotation( float yaw, float pitch )
	{
		if( m_yaw == yaw && m_pitch == pitch )
		{
			return;
		}

		m_dirty = true;

		// wrap the x direction
		m_yaw = dd::wrap( yaw, 0.0f, 360.0f );

		// clamp the y direction
		m_pitch = glm::clamp( pitch, -89.9f, 89.9f );

		float yawRads = glm::radians( m_yaw );
		float pitchRads = glm::radians( m_pitch );

		m_direction = glm::vec3( std::cos( pitchRads ) * std::sin( yawRads ),
			std::sin( pitchRads ),
			std::cos( pitchRads ) * std::cos( yawRads ) );
	}

	glm::vec3 FPSCamera::GetDirection() const
	{
		return m_direction;
	}
	
	void FPSCamera::SetDirection( const glm::vec3& dir )
	{
		if( m_direction == dir )
		{
			return;
		}

		m_dirty = true;

		m_direction = glm::normalize( dir );
	}

	float FPSCamera::GetNear() const
	{
		return m_near;
	}

	void FPSCamera::SetNear( float dist_near )
	{
		m_dirty = true;

		m_near = dist_near;
	}

	float FPSCamera::GetFar() const
	{
		return m_far;
	}

	void FPSCamera::SetFar( float dist_far )
	{
		m_dirty = true;

		m_far = dist_far;
	}

	float FPSCamera::GetVerticalFOV() const
	{
		return m_vfov;
	}

	void FPSCamera::SetVerticalFOV( float vfov )
	{
		m_dirty = true;

		m_vfov = vfov;
	}

	float FPSCamera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	void FPSCamera::SetAspectRatio( int w, int h )
	{
		m_dirty = true;

		m_aspectRatio = float( w ) / float( h );
	}

	glm::mat4 FPSCamera::GetProjectionMatrix() const
	{
		float fov = 1.0f / glm::tan( m_vfov / 2.0f );
		return glm::mat4(
			fov / m_aspectRatio, 0.0f, 0.0f, 0.0f,
			0.0f, fov, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, m_near, 0.0f );
	}

	glm::mat4 FPSCamera::GetCameraMatrix() const
	{
		return glm::lookAt( m_position, m_position + m_direction, glm::vec3( 0, 1, 0 ) );
	}

	void FPSCamera::CopyValuesFrom( const FPSCamera& other )
	{
		m_vfov = other.m_vfov;
		m_aspectRatio = other.m_aspectRatio;
		m_near = other.m_near;
		m_far = other.m_far;
		m_direction = other.m_direction;
		m_position = other.m_position;
	}
}