#pragma once

#include "ComponentType.h"
#include "HitResult.h"

namespace dd
{
	struct HitState;

	struct BulletComponent
	{
		glm::vec3 Velocity;
		glm::vec3 HitPosition;
		
		float Age { 0.0f };
		float Lifetime { 10.0f };
		
		HitHandle PendingHit; 
		
		DD_COMPONENT;
	};
}