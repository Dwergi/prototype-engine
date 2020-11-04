//
// FluxEnemySystem.h
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#pragma once

#include "Random.h"

#include "ddc/EntityPrototype.h"
#include "ddc/System.h"

namespace flux
{
	struct FluxEnemySystem : ddc::System, dd::IDebugPanel
	{
		FluxEnemySystem(glm::ivec2 map_size);

		virtual void Initialize(ddc::EntityLayer& layer) override;
		virtual void Update(ddc::UpdateData& update_data) override;

	private:

		float m_lastSpawn { 0 };
		float m_spawnDelay { 5.0f };
		int m_maxEnemies { 16 };

		ddc::EntityPrototypeHandle m_enemyPrototype;
		dd::RandomFloat m_rngPosition;
		dd::RandomFloat m_rngAngle;
		glm::ivec2 m_mapSize;

		virtual const char* GetDebugTitle() const override { return "Enemies"; }
		virtual void DrawDebugInternal() override;

		glm::vec2 GenerateSpawnPosition();
	};
}