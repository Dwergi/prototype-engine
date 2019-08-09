#pragma once

#include "IGame.h"

namespace neut
{
	struct NeutrinoGame : dd::IGame
	{
		virtual void Initialize() override;
		virtual void Shutdown(const dd::GameUpdateData& update_data) override;
		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual const char* GetTitle() const override { return "Neutrino"; }

		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& spaces) override;
		virtual void RegisterRenderers(ddr::WorldRenderer& renderer) override;
		virtual void RegisterSystems(ddc::SystemManager& system_manager) override;
	};
}