//
// EntityVisualizer.h - Visualizes the data of the currently selected entity in ImGui.
// Copyright (C) Sebastian Nordgren 
// October 7th 2018
//

#pragma once

#include "IDebugPanel.h"

namespace dd
{
	struct EntityVisualizer : IDebugPanel
	{
		EntityVisualizer();

		virtual const char* GetDebugTitle() const override { return "Entity Data"; }

	private:
		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;
	};
}