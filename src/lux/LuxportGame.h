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
		virtual void RegisterRenderers(ddr::WorldRenderer& renderer) override;
		virtual void RegisterSystems(ddc::SystemManager& system_manager) override;
		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& entity_spaces) override;

		virtual void Initialize() override;
		virtual void Shutdown(const dd::GameUpdateData& update_data) override;
		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual const char* GetTitle() const override { return "Luxport"; }
	};
}