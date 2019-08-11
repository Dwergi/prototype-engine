//
// Physics2DSystem.h
// Copyright (C) Sebastian Nordgren 
// August 4th 2018
//

#pragma once

#include "System.h"

namespace d2d
{
	struct PhysicsSystem : ddc::System
	{
		PhysicsSystem();

		void Update(const ddc::UpdateData& update_data);
	};
}