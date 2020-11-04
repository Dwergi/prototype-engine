//
// FluxPlayerController.cpp - Controller for Flux player.
// Copyright (C) Sebastian Nordgren 
// July 21st 2020
//

#include "PCH.h"
#include "FluxPlayerController.h"

#include "FluxBulletComponent.h"
#include "FluxPlayerComponent.h"
#include "Input.h"
#include "SpriteSheet.h"

#include "ddc/EntityPrototype.h"
#include "ddc/ScratchEntity.h"

#include "d2d/SpriteComponent.h"
#include "d2d/SpriteTileSystem.h"
#include "d2d/Transform2DComponent.h"

#include <cmath>

namespace flux
{
	static dd::Service<dd::Input> s_input;
	static dd::Service<ddc::EntityPrototypeManager> s_prototypes;
	static dd::Service<ddr::SpriteSheetManager> s_spriteManager;

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
		DD_ASSERT(!m_bulletPrototype.IsValid());

		s_input->AddHeldHandler(dd::InputAction::DOWN);
		s_input->AddHeldHandler(dd::InputAction::UP);
		s_input->AddHeldHandler(dd::InputAction::LEFT);
		s_input->AddHeldHandler(dd::InputAction::RIGHT);
		s_input->AddHeldHandler(dd::InputAction::SHOOT);

		m_bulletPrototype = s_prototypes->Create("player_bullet");

		ddc::ScratchEntity scratch;
		scratch.AddTag(ddc::Tag::Visible);
		scratch.AddTag(ddc::Tag::Dynamic);

		d2d::Transform2DComponent& transform_cmp = scratch.Add<d2d::Transform2DComponent>();
		transform_cmp.Scale = { 1, 1 };

		d2d::SpriteComponent& sprite_cmp = scratch.Add<d2d::SpriteComponent>();
		sprite_cmp.Pivot = { 0.5f, 0.5f };

		ddr::SpriteSheetHandle spritesheet_h = s_spriteManager->Find("player_bullet.png"); 
		sprite_cmp.Sprite = spritesheet_h->Get(0);
		sprite_cmp.ZIndex = 8;

		flux::FluxBulletComponent& bullet_cmp = scratch.Add<flux::FluxBulletComponent>();
		bullet_cmp.HitCircle = ddm::Circle(glm::vec2(0.5f, 0.5f), 0.25f);

		ddc::EntityPrototype* prototype = m_bulletPrototype.Access();
		prototype->PopulateFromScratch(scratch);
	}

	void FluxPlayerController::Update(ddc::UpdateData& update_data)
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
			angle = ddm::wrap(angle, glm::radians(0.0f), glm::radians(360.0f));
			transform_cmp.Rotation = angle;

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
					ddc::ScratchEntity bullet = m_bulletPrototype->InstantiateScratch();
					
					d2d::Transform2DComponent* bullet_transform = bullet.Access<d2d::Transform2DComponent>();
					bullet_transform->Rotation = angle;
					bullet_transform->Position = transform_cmp.Position + dir_to_mouse * transform_cmp.Scale * 0.5f;
					bullet_transform->Scale = { 1, 1 };

					flux::FluxBulletComponent* bullet_cmp = bullet.Access<flux::FluxBulletComponent>();
					bullet_cmp->Velocity = dir_to_mouse * player_cmp.EquippedWeapon->BulletSpeed;
					bullet_cmp->Damage = player_cmp.EquippedWeapon->BulletDamage;
					bullet_cmp->Type = flux::BulletType::Friendly;

					d2d::SpriteComponent* sprite_cmp = bullet.Access<d2d::SpriteComponent>();

					update_data.CreateEntity(std::move(bullet));

					player_cmp.ShotCooldown = player_cmp.EquippedWeapon->ShotDelay;
				}
			}
		}
	}
}