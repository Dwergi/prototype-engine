#pragma once

#include "ComponentType.h"

namespace dd
{
	struct HitState;

	struct BulletComponent
	{
		glm::vec3 Velocity;
		
		float Age { 0.0f };
		float Lifetime { 10.0f };
		
		const HitState* PendingHit { nullptr }; 
		bool HitCompleted { false };

		DD_COMPONENT;
	};
}