//
// FluxBulletSystem.h
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#pragma once

#include "System.h"

namespace flux
{
	struct FluxBulletSystem : ddc::System, dd::IDebugPanel
	{
		FluxBulletSystem(glm::vec2 map_size);

		virtual void Initialize(ddc::EntityLayer& layer) override;
		virtual void Update(ddc::UpdateData& update_data) override;
		virtual void Shutdown(ddc::EntityLayer& layer) override;

	private:
		glm::vec2 m_mapSize;

		int m_liveBullets { 0 };

		virtual const char* GetDebugTitle() const override { return "Bullets";  }
		virtual void DrawDebugInternal() override;
	};
}