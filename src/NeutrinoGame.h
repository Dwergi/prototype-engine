#pragma once

#include "IGame.h"

namespace neutrino
{
	struct NeutrinoGame : dd::IGame
	{
		virtual void Initialize(ddc::EntitySpace& entities) override;
		virtual void Shutdown(ddc::EntitySpace& entities) override;
		virtual void Update(ddc::EntitySpace& entities) override;
		virtual void RenderUpdate(ddc::EntitySpace& entities) override;

		virtual const char* GetTitle() const override { return "Neutrino"; }
	};
}