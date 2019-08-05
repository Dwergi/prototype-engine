#pragma once

#include "IGame.h"

namespace lux
{
	struct LuxportGame : dd::IGame
	{
		virtual void Initialize(ddc::EntitySpace& entities) override;
		virtual void Shutdown(ddc::EntitySpace& entities) override;
		virtual void Update(dd::GameUpdateData& entities) override;
		virtual void RenderUpdate(dd::GameUpdateData& entities) override;

		virtual const std::vector<ddc::EntitySpace>& GetEntitySpaces() override;

		virtual const char* GetTitle() const { return "Luxport"; }
	};
}