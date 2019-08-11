#pragma once

#include "IGame.h"

namespace neut
{
	struct NeutrinoGame : dd::IGame
	{
	private:
		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void Update(const dd::GameUpdateData& update_data) override;

		virtual const char* GetTitle() const override { return "Neutrino"; }
		virtual ddr::ICamera& GetCamera() const override;

		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& spaces) override;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) override;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) override;
	};
}