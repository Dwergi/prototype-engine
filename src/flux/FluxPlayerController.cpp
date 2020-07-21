//
// FluxPlayerController.cpp - Controller for Flux player.
// Copyright (C) Sebastian Nordgren 
// July 21st 2020
//

#include "PCH.h"
#include "FluxPlayerController.h"
#include "FluxPlayerComponent.h"

#include "Input.h"

#include "d2d/SpriteTileSystem.h"
#include "d2d/Transform2DComponent.h"

#include <cmath>

namespace flux
{
	dd::Service<dd::Input> s_input;

	FluxPlayerController::FluxPlayerController(const d2d::SpriteTileSystem& tile_system) :
		ddc::System("Flux Player"),
		m_tileSystem(tile_system)
	{
		RequireWrite<FluxPlayerComponent>();
		RequireWrite<d2d::Transform2DComponent>();
	}

	FluxPlayerController::~FluxPlayerController()
	{

	}

	void FluxPlayerController::Initialize(ddc::EntityLayer& layer)
	{
		s_input->AddHeldHandler(dd::InputAction::DOWN);
		s_input->AddHeldHandler(dd::InputAction::UP);
		s_input->AddHeldHandler(dd::InputAction::LEFT);
		s_input->AddHeldHandler(dd::InputAction::RIGHT);
		s_input->AddHeldHandler(dd::InputAction::SHOOT);
	}

	DD_OPTIMIZE_OFF()

	void FluxPlayerController::Update(const ddc::UpdateData& update_data)
	{
		DD_PROFILE_SCOPED(FluxPlayerController_Update);

		float dt = update_data.Delta();

		const ddc::UpdateBufferView& update_buffer = update_data.Data();
		auto transforms = update_buffer.Write<d2d::Transform2DComponent>();
		auto players = update_buffer.Write<FluxPlayerComponent>();

		dd::MousePosition mouse_pos = s_input->GetMousePosition();

		for (size_t i = 0; i < update_buffer.Size(); ++i)
		{
			d2d::Transform2DComponent& transform_cmp = transforms[i];
			flux::FluxPlayerComponent& player_cmp = players[i];

			glm::vec2 player_pos_px = m_tileSystem.CoordToPixels(transform_cmp.Position); 
			glm::vec2 dir_to_mouse = glm::normalize(mouse_pos.Absolute - player_pos_px);

			float angle = atan2(dir_to_mouse.y, dir_to_mouse.x) + glm::radians(90.0f);
			transform_cmp.Rotation = ddm::wrap(angle, glm::radians(0.0f), glm::radians(360.0f));

			glm::vec2 move_dir = { 0, 0 };
		
			if (s_input->IsHeld(dd::InputAction::LEFT))
			{
				move_dir.x -= 1;
			}
			if (s_input->IsHeld(dd::InputAction::RIGHT))
			{
				move_dir.x += 1;
			}
			if (s_input->IsHeld(dd::InputAction::UP))
			{
				move_dir.y -= 1; // top-left is 0,0
			}
			if (s_input->IsHeld(dd::InputAction::DOWN))
			{
				move_dir.y += 1;
			}

			// input is held down
			if (glm::length2(move_dir) > 0)
			{
				move_dir = glm::normalize(move_dir);
				player_cmp.Velocity += move_dir * player_cmp.Stats->Acceleration * dt;

				float new_speed = ddm::clamp(glm::length(player_cmp.Velocity), 0.0f, player_cmp.Stats->MaxSpeed);

				player_cmp.Velocity = move_dir * new_speed;
			}
			else
			{
				float old_speed = glm::length(player_cmp.Velocity);
				if (old_speed > 0)
				{
					glm::vec2 dir = player_cmp.Velocity / old_speed;

					float new_speed = old_speed - player_cmp.Stats->Deceleration * dt;
					new_speed = ddm::max(0.0f, new_speed);

					if (new_speed < 0.5f)
					{
						new_speed = 0.0f;
					}

					player_cmp.Velocity = dir * new_speed;
				}
			}

			transform_cmp.Position += player_cmp.Velocity * dt;
			transform_cmp.Update();
			
			player_cmp.ShotCooldown = std::max(player_cmp.ShotCooldown - dt, 0.0f);

			if (s_input->IsHeld(dd::InputAction::SHOOT))
			{
				if (player_cmp.ShotCooldown == 0)
				{
					DD_TODO("Spawn bullet.");
				}
			}
		}
	}
}