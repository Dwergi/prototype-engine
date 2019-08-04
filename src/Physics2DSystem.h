#pragma once

#include "System.h"

namespace dd
{
	struct Physics2DSystem : ddc::System
	{
		Physics2DSystem();

		void Update(const ddc::UpdateData& update_data);
	};
}