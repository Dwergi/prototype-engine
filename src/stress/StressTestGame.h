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
		int m_entityCount { 0 };
		int m_createCount { 0 };
		bool m_physics { true };

		// IDebugPanel implementation
		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const override { return "Stress Test"; }

		// IGame implementation
		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual void CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers) override;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;

		virtual const char* GetTitle() const override { return "Stress Test"; }
	};
}