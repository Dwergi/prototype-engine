//
// FluxPlayerController.h - Controller for Flux player.
// Copyright (C) Sebastian Nordgren 
// July 21st 2020
//

#pragma once

#include "System.h"

namespace d2d
{
	struct SpriteTileSystem;
}

namespace flux
{
	struct FluxPlayerController : ddc::System
	{
		FluxPlayerController(const d2d::SpriteTileSystem& tile_system);
		~FluxPlayerController();

		FluxPlayerController(const FluxPlayerController&) = delete;
		FluxPlayerController& operator=(const FluxPlayerController&) = delete; 

		virtual void Initialize(ddc::EntityLayer& layer) override;
		virtual void Update(const ddc::UpdateData& data) override;

	private:

		const d2d::SpriteTileSystem& m_tileSystem;
	};
}