#pragma once

#include "System.h"

namespace dd
{
	struct SpriteAnimationSystem : ddc::System
	{
		SpriteAnimationSystem();

		virtual void Update(const ddc::UpdateData& update_data) override;
	};
}