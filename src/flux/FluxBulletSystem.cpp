//
// FluxBulletSystem.cpp
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#include "PCH.h"
#include "FluxBulletSystem.h"

#include "d2d/Transform2DComponent.h"

#include "ddm/AABB2D.h"
#include "ddm/HitTest.h"

#include "flux/FluxBulletComponent.h"
#include "flux/FluxEnemyComponent.h"

namespace flux
{
	FluxBulletSystem::FluxBulletSystem(glm::vec2 map_size) :
		ddc::System("Bullets"),
		m_mapSize(map_size)
	{
		RequireWrite<d2d::Transform2DComponent>("bullets");
		RequireWrite<flux::FluxBulletComponent>("bullets");

		RequireRead<d2d::Transform2DComponent>("enemies");
		RequireWrite<flux::FluxEnemyComponent>("enemies");
	}

	void FluxBulletSystem::Initialize(ddc::EntityLayer& layer)
	{

	}

	void FluxBulletSystem::Shutdown(ddc::EntityLayer& layer)
	{

	}

	void FluxBulletSystem::DrawDebugInternal()
	{
		ImGui::Value("Live", m_liveBullets);
	}

	DD_OPTIMIZE_OFF();

	void FluxBulletSystem::Update(ddc::UpdateData& update_data)
	{
		float dt = update_data.Delta(); 

		const ddc::UpdateBufferView& bullets_buffer = update_data.Data("bullets");
		ddc::WriteView<d2d::Transform2DComponent> bullet_transforms = bullets_buffer.Write<d2d::Transform2DComponent>();
		ddc::WriteView<flux::FluxBulletComponent> bullet_cmps = bullets_buffer.Write<flux::FluxBulletComponent>();
		const dd::Span<ddc::Entity>& bullet_entities = bullets_buffer.Entities();

		const ddc::UpdateBufferView& enemies_buffer = update_data.Data("enemies");
		ddc::ReadView<d2d::Transform2DComponent> enemy_transforms = enemies_buffer.Read<d2d::Transform2DComponent>();
		ddc::WriteView<flux::FluxEnemyComponent> enemy_cmps = enemies_buffer.Write<flux::FluxEnemyComponent>();

		for (int b = 0; b < bullets_buffer.Size(); ++b)
		{
			d2d::Transform2DComponent& bullet_transform = bullet_transforms[b];
			flux::FluxBulletComponent& bullet_cmp = bullet_cmps[b];

			glm::vec2 prev_position = bullet_transform.Position;

			bullet_transform.Position += bullet_cmp.Velocity * dt;
			bullet_transform.Update();

			bullet_cmp.Lifetime += dt;

			ddm::Circle bullet_circle = bullet_cmp.HitCircle.GetTransformed(bullet_transform.Position, bullet_transform.Scale);
			static const ddm::AABB2D MAP_BOUNDS(glm::vec2(0, 0), m_mapSize);

			if (bullet_cmp.Type == flux::BulletType::Friendly)
			{
				for (int e = 0; e < enemies_buffer.Size(); ++e)
				{
					const d2d::Transform2DComponent& enemy_transform = enemy_transforms[e];
					flux::FluxEnemyComponent& enemy_cmp = enemy_cmps[e];

					ddm::Circle enemy_circle = enemy_cmp.HitCircle.GetTransformed(enemy_transform.Position, enemy_transform.Scale);

					// bullet is moving, so we get the distance to the swept circle formed by previous -> current position
					float distance = ddm::DistanceToSegment(enemy_circle.Centre, prev_position, bullet_transform.Position);
					if (distance <= (bullet_circle.Radius + enemy_circle.Radius))
					{
						// collision!
						enemy_cmp.Health -= bullet_cmp.Damage;

						update_data.DestroyEntity(bullet_entities[b]);
						break;
					}
				}
			}

			if (!MAP_BOUNDS.Intersects(bullet_circle))
			{
				update_data.DestroyEntity(bullet_entities[b]);
			}
		}
	}
}