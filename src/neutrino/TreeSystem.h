//
// TreeSystem.h - System for updating trees.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#pragma once

#include "IDebugPanel.h"

#include "ddc/System.h"

namespace neut
{
	struct TreeSystem : ddc::System, dd::IDebugPanel
	{
		TreeSystem();

		virtual void Initialize( ddc::EntityLayer& entities );
		virtual void Update( ddc::UpdateData& update_data );

		virtual const char* GetDebugTitle() const override { return "Trees"; }

	private:

		virtual void DrawDebugInternal();
	};
}