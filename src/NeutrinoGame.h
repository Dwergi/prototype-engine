#pragma once

#include "IGame.h"

namespace neutrino
{
	struct NeutrinoGame : dd::IGame
	{
		virtual void Initialize(ddc::World& world) override;
		virtual void Shutdown(ddc::World& world) override;
		virtual void Update(ddc::World& world) override;
		virtual void RenderUpdate(ddc::World& world) override;

		virtual const char* GetTitle() const override { return "Neutrino"; }
	};
}