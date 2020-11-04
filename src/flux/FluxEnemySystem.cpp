//
// FluxEnemySystem.cpp
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#include "PCH.h"
#include "FluxEnemySystem.h"

#include "SpriteSheet.h"

#include "d2d/SpriteComponent.h"
#include "d2d/Transform2DComponent.h"

#include "ddc/ScratchEntity.h"

#include "flux/FluxEnemyComponent.h"

namespace flux
{
	static dd::Service<ddc::EntityPrototypeManager> s_prototypes;
	static dd::Service<ddr::SpriteSheetManager> s_spriteManager;

	const float EDGE_OFFSET = 2.0f;

	FluxEnemySystem::FluxEnemySystem(glm::ivec2 map_size) : 
		ddc::System("Enemies"),
		m_mapSize(map_size)
	{
		RequireWrite<d2d::Transform2DComponent>();
		RequireWrite<flux::FluxEnemyComponent>();
		RequireTag(ddc::Tag::Visible);

		m_rngPosition.SetMin(0);
		m_rngPosition.SetMax(4);

		m_rngAngle.SetMin(0);
		m_rngAngle.SetMax(359);
	}

	void FluxEnemySystem::Initialize(ddc::EntityLayer& layer)
	{
		DD_ASSERT(!m_enemyPrototype.IsValid());

		m_enemyPrototype = s_prototypes->Create("enemy");

		ddc::ScratchEntity scratch;
		scratch.AddTag(ddc::Tag::Visible);
		scratch.AddTag(ddc::Tag::Dynamic);
		
		flux::FluxEnemyComponent& enemy_cmp = scratch.Add<flux::FluxEnemyComponent>();
		enemy_cmp.Health = 1;
		enemy_cmp.HitCircle.Centre = { 0.5f, 0.5f };
		enemy_cmp.HitCircle.Radius = 0.5f;

		scratch.Add<d2d::Transform2DComponent>();
		d2d::SpriteComponent& sprite_cmp = scratch.Add<d2d::SpriteComponent>();
		sprite_cmp.Pivot = { 0.5f, 0.5f };

		ddr::SpriteSheetHandle spritesheet_h = s_spriteManager->Find("enemy.png");
		sprite_cmp.Sprite = spritesheet_h->Get(0);
		sprite_cmp.ZIndex = 7;

		m_enemyPrototype->PopulateFromScratch(scratch);
	}

	glm::vec2 FluxEnemySystem::GenerateSpawnPosition()
	{
		// we treat this value as a position around the circumference of the map going clockwise: top -> right -> bottom -> left
		float circumference = m_rngPosition.Next();

		glm::vec2 new_pos { 0, 0 };
		if (circumference < 1)
		{
			new_pos.x = EDGE_OFFSET + (m_mapSize.x - EDGE_OFFSET) * circumference;
			new_pos.y = EDGE_OFFSET;
		}

		circumference -= 1;

		if (circumference >= 0 && circumference < 1)
		{
			new_pos.x = m_mapSize.x - EDGE_OFFSET;
			new_pos.y = EDGE_OFFSET + (m_mapSize.y - EDGE_OFFSET) * circumference;
		}

		circumference -= 1;

		if (circumference >= 0 && circumference < 1)
		{
			new_pos.x = EDGE_OFFSET + (m_mapSize.x - EDGE_OFFSET) * circumference;
			new_pos.y = m_mapSize.y - EDGE_OFFSET;
		}

		circumference -= 1;

		if (circumference >= 0)
		{
			new_pos.x = EDGE_OFFSET;
			new_pos.y = EDGE_OFFSET + (m_mapSize.y - EDGE_OFFSET) * circumference;
		}

		return new_pos;
	}

	static bool CollidesWithExistingEnemy(const ddm::Circle& new_hit_circle, const ddc::WriteView<d2d::Transform2DComponent>& transforms, const ddc::WriteView<flux::FluxEnemyComponent>& enemies)
	{
		DD_ASSERT(transforms.Size() == enemies.Size());

		for (int i = 0; i < transforms.Size(); ++i)
		{
			const flux::FluxEnemyComponent& existing_enemy = enemies[i];
			const d2d::Transform2DComponent& existing_transform = transforms[i];

			ddm::Circle existing_circle = existing_enemy.HitCircle.GetTransformed(existing_transform.Position, existing_transform.Scale);
			if (new_hit_circle.Intersects(existing_circle))
			{
				return true;
			}
		}

		return false;
	}

	void FluxEnemySystem::Update(ddc::UpdateData& update_data)
	{
		float dt = update_data.Delta();

		const ddc::UpdateBufferView& update_buffer = update_data.Data();
		const ddc::WriteView<d2d::Transform2DComponent>& transforms = update_buffer.Write<d2d::Transform2DComponent>();
		const ddc::WriteView<flux::FluxEnemyComponent>& enemies = update_buffer.Write<flux::FluxEnemyComponent>();

		for (int i = 0; i < update_buffer.Size(); ++i)
		{
			flux::FluxEnemyComponent& enemy_cmp = enemies[i];

			if (enemy_cmp.Health <= 0)
			{
				ddc::Entity enemy_entity = update_buffer.Entities()[i];
				update_data.DestroyEntity(enemy_entity);
			}
		}

		m_lastSpawn -= dt;

		if (m_lastSpawn <= 0 && update_buffer.Size() < m_maxEnemies)
		{
			ddc::ScratchEntity new_enemy = m_enemyPrototype->InstantiateScratch();

			DD_ASSERT(new_enemy.HasTag(ddc::Tag::Visible));

			flux::FluxEnemyComponent* new_enemy_cmp = new_enemy.Access<flux::FluxEnemyComponent>();

			d2d::Transform2DComponent* new_transform_cmp = new_enemy.Access<d2d::Transform2DComponent>();

			glm::vec2 new_pos;
			ddm::Circle new_hit_circle;
			
			do
			{
				new_pos = GenerateSpawnPosition();

				new_hit_circle = new_enemy_cmp->HitCircle.GetTransformed(new_pos, glm::vec2(1, 1));
			} 
			while (CollidesWithExistingEnemy(new_hit_circle, transforms, enemies));

			new_transform_cmp->Position = new_pos;
			new_transform_cmp->Rotation = glm::radians(m_rngAngle.Next());
			new_transform_cmp->Update();

			update_data.CreateEntity(std::move(new_enemy));

			m_lastSpawn = m_spawnDelay;
		}
	}

	void FluxEnemySystem::DrawDebugInternal()
	{
		ImGui::SliderFloat("Spawn Delay", &m_spawnDelay, 0, 60);
		ImGui::SliderInt("Max Enemies", &m_maxEnemies, 0, 30);
	}
}