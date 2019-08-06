#pragma once

#include "System.h"

namespace d2d
{
	struct SpriteAnimationSystem : ddc::System
	{
		SpriteAnimationSystem();

		virtual void Update(const ddc::UpdateData& update_data) override;
	};
}