#include "PCH.h"
#include "ShakyCamera.h"

#include "FPSCameraComponent.h"
#include "InputBindings.h"

#include "glm/gtc/noise.hpp"

namespace dd
{
	float ShakyCamera::TraumaDecayRate = 0.5f;
	float ShakyCamera::MaximumYaw = 1.0f;
	float ShakyCamera::MaximumPitch = 1.0f;
	float ShakyCamera::MaximumRoll = 5.0f;
	float ShakyCamera::SpeedMultiplier = 25.0f;
	
	ShakyCamera::ShakyCamera( const FPSCameraComponent& camera, InputBindings& bindings ) :
		m_sourceCamera( camera )
	{
		bindings.RegisterHandler( dd::InputAction::ADD_MINOR_TRAUMA, [this]( InputAction action, InputType type ) 
		{ 
			AddTraumaHandler( action, type ); 
		} );

		bindings.RegisterHandler( dd::InputAction::ADD_MAJOR_TRAUMA, [this]( InputAction action, InputType type )
		{
			AddTraumaHandler( action, type );
		} );
	}

	ShakyCamera::~ShakyCamera()
	{

	}
	
	void ShakyCamera::AddTraumaHandler( InputAction action, InputType type )
	{
		if( action == InputAction::ADD_MINOR_TRAUMA && type == InputType::RELEASED )
		{
			AddTrauma( 0.25f );
		}

		if( action == InputAction::ADD_MAJOR_TRAUMA && type == InputType::RELEASED )
		{
			AddTrauma( 1.0f );
		}
	}

	void ShakyCamera::Update( float delta_t )
	{
		m_time += delta_t * SpeedMultiplier;

		if( m_trauma > 0 )
		{
			float shake = m_trauma * m_trauma;

			float extraYaw = MaximumYaw * shake * glm::simplex( glm::vec2( m_time, Seed ) );
			float yaw = m_sourceCamera.GetYaw() + glm::radians( extraYaw );
			m_yaw = ddm::wrap( yaw, 0.0f, glm::two_pi<float>() );

			float extraPitch = MaximumPitch * shake * glm::simplex( glm::vec2( m_time, Seed + 1 ) );
			float pitch = m_sourceCamera.GetPitch() + glm::radians( extraPitch );

			const float max_pitch = glm::half_pi<float>() - 0.00001f;

			m_pitch = glm::clamp( pitch, -max_pitch, max_pitch );

			m_roll = glm::radians( MaximumRoll * shake * glm::simplex( glm::vec2( m_time, Seed + 2 ) ) );
		}
		else
		{
			m_yaw = m_sourceCamera.GetYaw();
			m_pitch = m_sourceCamera.GetPitch();
			m_roll = 0;
		}

		m_direction = ddm::DirectionFromPitchYaw( m_pitch, m_yaw );

		AddTrauma( -(delta_t * TraumaDecayRate) );

		m_frustum.Update( *this );
	}

	void ShakyCamera::AddTrauma( float amount )
	{
		m_trauma += amount;
		m_trauma = glm::clamp( m_trauma, 0.0f, 1.0f );
	}

	void ShakyCamera::DrawDebugInternal( ddc::World& world )
	{
		ImGui::ProgressBar( m_trauma );
		ImGui::DragFloat( "Decay Rate", &ShakyCamera::TraumaDecayRate, 0.01f, 0.0f, 1.0f );
		ImGui::DragFloat( "Speed Multiplier", &ShakyCamera::SpeedMultiplier, 0.1f, 0.0f, 50.0f );
		ImGui::DragFloat( "Maximum Yaw (deg)", &ShakyCamera::MaximumYaw, 0.1f, 0.0f, 50.0f );
		ImGui::DragFloat( "Maximum Pitch (deg)", &ShakyCamera::MaximumPitch, 0.1f, 0.0f, 50.0f );
		ImGui::DragFloat( "Maximum Roll (deg)", &ShakyCamera::MaximumRoll, 0.1f, 0.0f, 50.0f );
	}

	float ShakyCamera::GetVerticalFOV() const
	{
		return m_sourceCamera.GetVerticalFOV();
	}

	float ShakyCamera::GetAspectRatio() const
	{
		return m_sourceCamera.GetAspectRatio();
	}

	float ShakyCamera::GetNear() const
	{
		return m_sourceCamera.GetNear();
	}

	float ShakyCamera::GetFar() const
	{
		return m_sourceCamera.GetFar();
	}

	float ShakyCamera::GetYaw() const
	{
		return m_sourceCamera.GetYaw();
	}

	float ShakyCamera::GetPitch() const
	{
		return m_sourceCamera.GetPitch();
	}

	glm::vec3 ShakyCamera::GetPosition() const
	{
		return m_sourceCamera.GetPosition();
	}

	glm::mat4 ShakyCamera::GetProjectionMatrix() const
	{
		return m_sourceCamera.GetProjectionMatrix();
	}

	glm::mat4 ShakyCamera::GetViewMatrix() const
	{
		glm::vec4 up = glm::rotate( m_roll, glm::vec3( 0, 0, 1 ) ) * glm::vec4( 0, 1, 0, 0 );

		return glm::lookAt( m_sourceCamera.GetPosition(), m_sourceCamera.GetPosition() + m_sourceCamera.GetDirection(), up.xyz() );
	}
}