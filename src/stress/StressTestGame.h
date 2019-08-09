//
// StressTestGame.h - Game file for engine stress test.
// Copyright (C) Sebastian Nordgren 
// August 9th 2019
//

#pragma once

#include "IGame.h"
#include "IDebugPanel.h"

namespace stress
{
	struct StressTestGame : dd::IGame, dd::IDebugPanel
	{
		virtual void Initialize() override;
		virtual void Shutdown(const dd::GameUpdateData& update_data) override;
		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& entity_spaces) override;
		virtual void RegisterRenderers(ddr::WorldRenderer& renderer) override;
		virtual void RegisterSystems(ddc::SystemManager& system_manager) override;

		virtual const char* GetTitle() const override { return "Stress Test"; }
		virtual const char* GetDebugTitle() const override { return "Stress Test"; }

	private:
		virtual void DrawDebugInternal(ddc::EntitySpace& entities) override;

		int m_entityCount { 1024 };
		int m_createCount { 0 };
	};
}