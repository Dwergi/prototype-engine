#pragma once

#include "IGame.h"

namespace phys2d
{
	struct Physics2DGame : dd::IGame
	{
		// Inherited via IGame
		virtual void Initialize() override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void CreateEntityLayers(std::vector<ddc::EntityLayer*>& spaces) override;
		virtual void Shutdown() override;
		virtual void Update(const dd::GameUpdateData& update_data) override;
	};
}