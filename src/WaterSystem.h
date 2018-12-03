//
// WaterSystem.h
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#pragma once

#include "System.h"

namespace dd
{
	struct WaterSystem : ddc::System
	{
		WaterSystem();

		virtual void Update( const ddc::UpdateData& update_data ) override;
	};
}