//
// FreeCameraController.h - Controller class for a free camera.
// Copyright (C) Sebastian Nordgren 
// April 12th 2015
//

#include "PCH.h"
#include "FreeCameraController.h"

#include "FPSCameraComponent.h"
#include "Input.h"
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

	static dd::Service<dd::Input> s_input;

	FreeCameraController::FreeCameraController() :
		ddc::System("Free Camera")
	{
		m_mouseDelta = glm::vec2(0, 0);

		RequireWrite<FPSCameraComponent>();
		RequireWrite<TransformComponent>();
	}

	FreeCameraController::FreeCameraController(FreeCameraController&& other) :
		ddc::System("Free Camera"),
		m_enabled(other.m_enabled),
		m_invert(other.m_invert)
	{
	}

	FreeCameraController::~FreeCameraController()
	{

	}

	void FreeCameraController::DrawDebugInternal()
	{
		ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 30), ImGuiCond_FirstUseEver);

		ImGui::Checkbox("Enabled", &m_enabled);

		for (size_t i = 0; i < m_cameras.size(); ++i)
		{
			FPSCameraComponent* camera = m_cameras[i];

			std::string label = fmt::format("Camera {}", i);
			if (ImGui::CollapsingHeader(label.c_str()))
			{

				ImGui::Value("Yaw", glm::degrees(camera->GetYaw()), "%.2f");
				ImGui::Value("Pitch", glm::degrees(camera->GetPitch()), "%.2f");

				glm::vec3 position = camera->GetPosition();
				ImGui::Value("Position", position, "%.1f");

				glm::vec3 direction = camera->GetDirection();
				ImGui::Value("Direction", direction, "%.2f");

				float vfov = glm::degrees(camera->GetVerticalFOV() * 2.0f);
				if (ImGui::SliderFloat("VFOV", &vfov, 0.1f, 178.0f, "%.2f"))
				{
					camera->SetVerticalFOV(glm::radians(vfov / 2.0f));
				}

				float near_distance = camera->GetNear();
				float far_distance = camera->GetFar();
				if (ImGui::SliderFloat("Near", &near_distance, 0.01f, far_distance - 0.01f, "%.2f"))
				{
					camera->SetNear(near_distance);
				}

				if (ImGui::SliderFloat("Far", &far_distance, near_distance + 0.01f, 5000.f, "%.2f"))
				{
					camera->SetFar(far_distance);
				}

				ImGui::Checkbox("Invert", &m_invert);
			}
		}
	}

	void FreeCameraController::Initialize(ddc::EntityLayer& layer)
	{
		s_input->AddHeldHandler(InputAction::FORWARD);
		s_input->AddHeldHandler(InputAction::BACKWARD);
		s_input->AddHeldHandler(InputAction::LEFT);
		s_input->AddHeldHandler(InputAction::RIGHT);
		s_input->AddHeldHandler(InputAction::UP);
		s_input->AddHeldHandler(InputAction::DOWN);
		s_input->AddHeldHandler(InputAction::BOOST);
	}

	void FreeCameraController::Update(ddc::UpdateData& update_data)
	{
		DD_PROFILE_SCOPED(FreeCameraController_Update);

		if (s_input->GetCurrentMode() != "game")
		{
			return;
		}

		m_cameras.clear();

		float dt = update_data.Delta();

		const ddc::UpdateBufferView& data = update_data.Data();
		auto cameras = data.Write<FPSCameraComponent>();
		auto transforms = data.Write<TransformComponent>();

		dd::MousePosition mouse_pos = s_input->GetMousePosition();
		dd::MousePosition mouse_scroll = s_input->GetMouseScroll();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			FPSCameraComponent& camera = cameras[i];
			TransformComponent& transform = transforms[i];

			m_cameras.push_back(&camera);

			float yaw = camera.GetYaw();
			yaw += glm::radians(mouse_pos.Delta.x * TurnSpeed);

			float y_delta = glm::radians(mouse_pos.Delta.y * TurnSpeed);

			if (m_invert)
				y_delta = -y_delta;

			float pitch = camera.GetPitch();
			pitch += y_delta;

			camera.SetRotation(yaw, pitch);

			glm::vec3 direction = camera.GetDirection();
			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 right = glm::normalize(glm::cross(direction, up));

			glm::vec3 movement(0, 0, 0);

			if (s_input->IsHeld(InputAction::FORWARD))
				movement += direction;

			if (s_input->IsHeld(InputAction::BACKWARD))
				movement -= direction;

			if (s_input->IsHeld(InputAction::LEFT))
				movement -= right;

			if (s_input->IsHeld(InputAction::RIGHT))
				movement += right;

			if (s_input->IsHeld(InputAction::UP))
				movement += up;

			if (s_input->IsHeld(InputAction::DOWN))
				movement -= up;

			if (glm::length(movement) > 0)
			{
				// normalize direction
				movement = glm::normalize(movement);

				if (s_input->IsHeld(InputAction::BOOST))
					movement *= BoostMultiplier;

				// scale with time and speed
				glm::vec3 scaled = movement * MovementSpeed * dt;

				glm::vec3 position = camera.GetPosition();
				position += scaled;
				camera.SetPosition(position);

				transform.Position = position;
			}

			{
				float vfov = camera.GetVerticalFOV();

				float degs = glm::degrees(vfov);

				degs *= std::powf(2.f, -mouse_scroll.Delta.y * ZoomSpeed);
				degs = glm::clamp(degs, 5.f, 89.f);

				camera.SetVerticalFOV(glm::radians(degs));
			}

			transform.Rotation = ddm::QuatFromPitchYaw(pitch, yaw);
			transform.Update();

			camera.Update(dt);
		}
	}
}