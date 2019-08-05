//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PCH.h"
#include "FreeCameraController.h"

#include "FPSCameraComponent.h"
#include "InputKeyBindings.h"
#include "TransformComponent.h"

#include "fmt/format.h"

#include "glm/gtc/quaternion.hpp"

namespace dd
{
	// camera movement speed in meters per second
	const float MovementSpeed = 10.0f;
	const float BoostMultiplier = 5.0f;
	const float ZoomSpeed = 0.1f;

	// mouse sensitivity - 3840 pixels turns 180 degrees
	const float TurnSpeed = 180.f / 3840.f;

	FreeCameraController::FreeCameraController() :
		ddc::System( "Free Camera" )
	{
		m_mouseDelta = glm::vec2( 0, 0 );

		m_inputs.Add( InputAction::FORWARD, false );
		m_inputs.Add( InputAction::BACKWARD, false );
		m_inputs.Add( InputAction::LEFT, false );
		m_inputs.Add( InputAction::RIGHT, false );
		m_inputs.Add( InputAction::UP, false );
		m_inputs.Add( InputAction::DOWN, false );
		m_inputs.Add( InputAction::BOOST, false );

		RequireWrite<FPSCameraComponent>();
		RequireWrite<TransformComponent>();
	}

	FreeCameraController::FreeCameraController( FreeCameraController&& other ) :
		ddc::System( "Free Camera" ),
		m_inputs( std::move( other.m_inputs ) ),
		m_enabled( other.m_enabled ),
		m_invert( other.m_invert )
	{
	}

	FreeCameraController::~FreeCameraController()
	{

	}

	void FreeCameraController::BindActions( InputKeyBindings& bindings )
	{
		auto handle_input = [this]( InputAction action, InputType type )
		{
			HandleInput( action, type );
		};

		DD_TODO("Fix key handler bindings");
		/*bindings.RegisterHandler( InputAction::FORWARD, handle_input );
		bindings.RegisterHandler( InputAction::BACKWARD, handle_input );
		bindings.RegisterHandler( InputAction::LEFT, handle_input );
		bindings.RegisterHandler( InputAction::RIGHT, handle_input );
		bindings.RegisterHandler( InputAction::UP, handle_input );
		bindings.RegisterHandler( InputAction::DOWN, handle_input );
		bindings.RegisterHandler( InputAction::BOOST, handle_input );*/
	}

	void FreeCameraController::HandleInput( InputAction action, InputType type )
	{
		if( !m_enabled )
			return;

		bool* state = m_inputs.Find( action );
		if( state == nullptr )
			return;

		if( type == InputType::Press )
			*state = true;

		if( type == InputType::Release )
			*state = false;
	}

	void FreeCameraController::DrawDebugInternal( ddc::EntitySpace& entities )
	{
		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - 300, 30 ), ImGuiCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );

		for( size_t i = 0; i < m_cameras.size(); ++i )
		{
			FPSCameraComponent* camera = m_cameras[i];

			std::string label = fmt::format( "Camera {}", i );
			if( ImGui::TreeNodeEx( label.c_str(), ImGuiTreeNodeFlags_CollapsingHeader ) )
			{

				ImGui::Value( "Yaw", glm::degrees( camera->GetYaw() ), "%.2f" );
				ImGui::Value( "Pitch", glm::degrees( camera->GetPitch() ), "%.2f" );

				glm::vec3 position = camera->GetPosition();
				ImGui::Value( "Position", position, "%.1f" );

				glm::vec3 direction = camera->GetDirection();
				ImGui::Value( "Direction", direction, "%.2f" );

				float vfov = glm::degrees( camera->GetVerticalFOV() * 2.0f );
				if( ImGui::SliderFloat( "VFOV", &vfov, 0.1f, 178.0f, "%.2f" ) )
				{
					camera->SetVerticalFOV( glm::radians( vfov / 2.0f ) );
				}

				float near_distance = camera->GetNear();
				float far_distance = camera->GetFar();
				if( ImGui::SliderFloat( "Near", &near_distance, 0.01f, far_distance - 0.01f, "%.2f" ) )
				{
					camera->SetNear( near_distance );
				}

				if( ImGui::SliderFloat( "Far", &far_distance, near_distance + 0.01f, 5000.f, "%.2f" ) )
				{
					camera->SetFar( far_distance );
				}

				ImGui::Checkbox( "Invert", &m_invert );

				ImGui::TreePop();
			}
		}
	}
	
	void FreeCameraController::Update( const ddc::UpdateData& update_data )
	{
		DD_PROFILE_SCOPED( FreeCameraController_Update );

		m_cameras.clear();

		float dt = update_data.Delta();

		auto data = update_data.Data();
		auto cameras = data.Write<FPSCameraComponent>();
		auto transforms = data.Write<TransformComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			FPSCameraComponent& camera = cameras[i];
			TransformComponent& transform = transforms[i];

			m_cameras.push_back( &camera );

			float yaw = camera.GetYaw();
			yaw += glm::radians( m_mouseDelta.x * TurnSpeed );

			float y_delta = glm::radians( m_mouseDelta.y * TurnSpeed );

			if( m_invert )
				y_delta = -y_delta;

			float pitch = camera.GetPitch();
			pitch += y_delta;

			camera.SetRotation( yaw, pitch );

			glm::vec3 direction = camera.GetDirection();
			glm::vec3 up = glm::vec3( 0, 1, 0 );
			glm::vec3 right = glm::normalize( glm::cross( direction, up ) );

			glm::vec3 movement( 0, 0, 0 );

			if( m_inputs[InputAction::FORWARD] )
				movement += direction;

			if( m_inputs[InputAction::BACKWARD] )
				movement -= direction;

			if( m_inputs[InputAction::LEFT] )
				movement -= right;

			if( m_inputs[InputAction::RIGHT] )
				movement += right;

			if( m_inputs[InputAction::UP] )
				movement += up;

			if( m_inputs[InputAction::DOWN] )
				movement -= up;

			if( glm::length( movement ) > 0 )
			{
				// normalize direction
				movement = glm::normalize( movement );

				if( m_inputs[InputAction::BOOST] )
					movement *= BoostMultiplier;

				// scale with time and speed
				glm::vec3 scaled = movement * MovementSpeed * dt;

				glm::vec3 position = camera.GetPosition();
				position += scaled;
				camera.SetPosition( position );

				transform.Position = position;
			}

			{
				float vfov = camera.GetVerticalFOV();

				float degs = glm::degrees( vfov );

				degs *= std::powf( 2.f, -m_scrollDelta.y * ZoomSpeed );
				degs = glm::clamp( degs, 5.f, 89.f );

				camera.SetVerticalFOV( glm::radians( degs ) );
			}

			transform.Rotation = ddm::QuatFromPitchYaw( pitch, yaw );
			transform.Update();

			camera.Update( dt );
		}
	}

	void FreeCameraController::UpdateMouse( const MousePosition& pos )
	{
		m_mouseDelta = pos.Delta;
	}

	void FreeCameraController::UpdateScroll( const MousePosition& pos )
	{
		m_scrollDelta = pos.Delta;
	}
}