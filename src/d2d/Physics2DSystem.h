//
// Physics2DSystem.h
// Copyright (C) Sebastian Nordgren 
// August 4th 2018
//

#pragma once

#include "IDebugPanel.h"

#include "ddc/System.h"

namespace d2d
{
	struct PhysicsSystem : ddc::System, dd::IDebugPanel
	{
		PhysicsSystem();

		void Update(ddc::UpdateData& update_data);

	private:
		int m_collisionCount { 0 };

		// IDebugPanel implementation
		virtual const char* GetDebugTitle() const override { return "Physics 2D"; }
		virtual void DrawDebugInternal() override;
	};
}