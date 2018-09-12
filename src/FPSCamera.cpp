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

		m_position = pos;
	}

	void FPSCamera::SetRotation( float yaw, float pitch )
	{
		if( m_yaw == yaw && m_pitch == pitch )
		{
			return;
		}

		// wrap yaw
		m_yaw = dd::wrap( yaw, 0.0f, glm::two_pi<float>() );

		// clamp pitch to vertical up/down
		const float max_pitch = glm::half_pi<float>() - 0.00001f;
		m_pitch = glm::clamp( pitch, -max_pitch, max_pitch );

		m_direction = dd::directionFromPitchYaw( m_pitch, m_yaw );
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

		m_direction = glm::normalize( dir );
	}

	float FPSCamera::GetNear() const
	{
		return m_near;
	}

	void FPSCamera::SetNear( float dist_near )
	{
		if( dist_near != m_near )
		{
			m_near = dist_near;
		}
	}

	float FPSCamera::GetFar() const
	{
		return m_far;
	}

	void FPSCamera::SetFar( float dist_far )
	{
		if( dist_far != m_far )
		{
			m_far = dist_far;
		}
	}

	float FPSCamera::GetVerticalFOV() const
	{
		return m_vfov;
	}

	void FPSCamera::SetVerticalFOV( float vfov )
	{
		if( m_vfov != vfov )
		{
			m_vfov = vfov;
		}
	}

	float FPSCamera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	void FPSCamera::SetAspectRatio( int w, int h )
	{
		float aspectRatio = float( w ) / float( h );
		if( aspectRatio != m_aspectRatio )
		{
			m_aspectRatio = aspectRatio;
		}
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

	glm::mat4 FPSCamera::GetViewMatrix() const
	{
		return glm::lookAt( m_position, m_position + m_direction, glm::vec3( 0, 1, 0 ) );
	}

	void FPSCamera::Update( float delta_t )
	{
		m_frustum.Update( *this );
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