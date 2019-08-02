//
// FPSCameraComponent.h - A generic perspective FPSCamera thing.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PCH.h"
#include "FPSCameraComponent.h"

#include "Math_dd.h"
#include "IWindow.h"

#include <glm/gtc/matrix_transform.hpp>

DD_TYPE_CPP( dd::FPSCameraComponent );

namespace dd
{
	glm::vec3 FPSCameraComponent::GetPosition() const
	{
		return m_position;
	}

	void FPSCameraComponent::SetPosition( const glm::vec3& pos )
	{
		if( m_position == pos )
		{
			return;
		}

		m_position = pos;
	}

	void FPSCameraComponent::SetRotation( float yaw, float pitch )
	{
		if( m_yaw == yaw && m_pitch == pitch )
		{
			return;
		}

		// wrap yaw
		m_yaw = ddm::wrap( yaw, 0.0f, glm::two_pi<float>() );

		// clamp pitch to vertical up/down
		const float max_pitch = glm::half_pi<float>() - 0.00001f;
		m_pitch = glm::clamp( pitch, -max_pitch, max_pitch );
	}

	glm::vec3 FPSCameraComponent::GetDirection() const
	{
		return ddm::DirectionFromPitchYaw( m_pitch, m_yaw );
	}

	void FPSCameraComponent::SetDirection( const glm::vec3& dir )
	{
		ddm::PitchYawFromDirection( glm::normalize( dir ), m_pitch, m_yaw );
	}

	float FPSCameraComponent::GetNear() const
	{
		return m_near;
	}

	void FPSCameraComponent::SetNear( float dist_near )
	{
		if( dist_near != m_near )
		{
			m_near = dist_near;
		}
	}

	float FPSCameraComponent::GetFar() const
	{
		return m_far;
	}

	void FPSCameraComponent::SetFar( float dist_far )
	{
		if( dist_far != m_far )
		{
			m_far = dist_far;
		}
	}

	float FPSCameraComponent::GetVerticalFOV() const
	{
		return m_vfov;
	}

	void FPSCameraComponent::SetVerticalFOV( float vfov )
	{
		if( m_vfov != vfov )
		{
			m_vfov = vfov;
		}
	}

	float FPSCameraComponent::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	void FPSCameraComponent::SetSize(glm::ivec2 size)
	{
		float aspectRatio = (float) size.x / (float) size.y;
		if( aspectRatio != m_aspectRatio )
		{
			m_aspectRatio = aspectRatio;
		}
	}

	glm::mat4 FPSCameraComponent::GetProjectionMatrix() const
	{
		float fov = 1.0f / glm::tan( m_vfov / 2.0f );
		return glm::mat4(
			fov / m_aspectRatio, 0.0f, 0.0f, 0.0f,
			0.0f, fov, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, m_near, 0.0f );
	}

	glm::mat4 FPSCameraComponent::GetViewMatrix() const
	{
		return glm::lookAt( m_position, m_position + GetDirection(), glm::vec3( 0, 1, 0 ) );
	}

	void FPSCameraComponent::Update( float delta_t )
	{
		m_frustum.Update( *this );
	}

	void FPSCameraComponent::CopyValuesFrom( const FPSCameraComponent& other )
	{
		m_vfov = other.m_vfov;
		m_aspectRatio = other.m_aspectRatio;
		m_near = other.m_near;
		m_far = other.m_far;
		m_position = other.m_position;
	}
}