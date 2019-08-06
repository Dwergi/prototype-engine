#pragma once

#include "IGame.h"

namespace neut
{
	struct NeutrinoGame : dd::IGame
	{
		virtual void Initialize(const dd::GameUpdateData& update_data) override;
		virtual void Shutdown(const dd::GameUpdateData& update_data) override;
		virtual void Update(const dd::GameUpdateData& update_data) override;
		virtual void RenderUpdate(const dd::GameUpdateData& update_data) override;

		virtual const char* GetTitle() const override { return "Neutrino"; }

		virtual const std::vector<ddc::EntitySpace*>& GetEntitySpaces() const;
	};
}