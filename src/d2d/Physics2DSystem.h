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