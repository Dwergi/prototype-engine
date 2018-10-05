#pragma once

#include "IDebugPanel.h"

namespace dd
{
	struct EntityVisualizer : IDebugPanel
	{
		virtual const char* GetDebugTitle() const override { return "Entity Data"; }

	private:
		virtual void DrawDebugInternal( ddc::World& world ) override;
	};
}