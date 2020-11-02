//
// EntityVisualizer.h - Visualizes the data of the currently selected entity in ImGui.
// Copyright (C) Sebastian Nordgren 
// October 7th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"

namespace dd
{
	struct EntityVisualizer : IDebugPanel, ddc::System
	{
		EntityVisualizer();

		// Inherited via System
		virtual void Update(ddc::UpdateData& update_data) override;

		virtual const char* GetDebugTitle() const override { return "Entity Data"; }

	private:
		virtual void DrawDebugInternal() override;

		ddc::Entity m_selected;
	};
}