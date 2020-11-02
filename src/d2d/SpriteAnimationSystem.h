#pragma once

#include "System.h"

namespace d2d
{
	struct SpriteAnimationSystem : ddc::System
	{
		SpriteAnimationSystem();

		virtual void Update(ddc::UpdateData& update_data) override;
	};
}