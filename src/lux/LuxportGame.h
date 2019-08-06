#pragma once

#include "IGame.h"

namespace lux
{
	struct LuxportGame : dd::IGame
	{
		virtual void Initialize(const dd::GameUpdateData& entities) override;
		virtual void Shutdown(const dd::GameUpdateData& entities) override;
		virtual void Update(const dd::GameUpdateData& entities) override;
		virtual void RenderUpdate(const dd::GameUpdateData& entities) override;

		virtual const std::vector<ddc::EntitySpace*>& GetEntitySpaces() const override;

		virtual const char* GetTitle() const { return "Luxport"; }
	};
}