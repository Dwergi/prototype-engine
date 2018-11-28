//
// TreeSystem.h - System for updating trees.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"

namespace dd
{
	struct TreeSystem : ddc::System, IDebugPanel
	{
		TreeSystem();

		virtual void Initialize( ddc::World& world );
		virtual void Update( const ddc::UpdateData& update_data );

		virtual const char* GetDebugTitle() const override { return "Trees"; }

	private:

		virtual void DrawDebugInternal( ddc::World& world );
	};
}