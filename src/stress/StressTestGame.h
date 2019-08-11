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
	private:
		int m_entityCount { 1024 };
		int m_createCount { 0 };

		// IDebugPanel implementation
		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const override { return "Stress Test"; }

		// IGame implementation
		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& entity_spaces) override;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;

		virtual const char* GetTitle() const override { return "Stress Test"; }

		virtual ddr::ICamera& GetCamera() const override;
	};
}