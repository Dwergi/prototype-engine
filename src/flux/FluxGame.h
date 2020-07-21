#pragma once

#include "IGame.h"
#include "IDebugPanel.h"

namespace flux
{
	struct FluxGame : dd::IGame, dd::IDebugPanel
	{
		// Initialization occurs in this order.
		virtual void Initialize() override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers) override;

		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual void Shutdown() override;

		virtual const char* GetTitle() const { return "Flux"; }

	private:
		virtual const char* GetDebugTitle() const { return GetTitle(); }
		virtual void DrawDebugInternal() override;
	};
}