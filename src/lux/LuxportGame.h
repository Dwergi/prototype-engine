//
// LuxportGame.h - Game file for Luxport GMTK Game Jame 2019.
// Copyright (C) Sebastian Nordgren 
// August 3rd 2019
//

#pragma once

#include "IGame.h"

namespace lux
{
	struct LuxportGame : dd::IGame
	{
	private:
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;
		virtual void CreateEntityLayers(std::vector<ddc::EntityLayer*>& entity_layers) override;

		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual const char* GetTitle() const override { return "Luxport"; }
	};
}