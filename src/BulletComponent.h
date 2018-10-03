//
// PhysicsPlaneComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "HitResult.h"

namespace dd
{
	struct HitState;

	struct BulletHitMessage
	{
		glm::vec3 Position;
		glm::vec3 SurfaceNormal;
		glm::vec3 Velocity;

		DD_CLASS( dd::BulletHitMessage )
		{
			DD_MEMBER( dd::BulletHitMessage, Position );
			DD_MEMBER( dd::BulletHitMessage, SurfaceNormal );
			DD_MEMBER( dd::BulletHitMessage, Velocity );
		}
	};

	struct BulletComponent
	{
		glm::vec3 Velocity;
		glm::vec3 HitPosition;
		glm::vec3 HitNormal;
		
		float Age { 0.0f };
		float Lifetime { 10.0f };
		
		HitHandle PendingHit; 
		
		DD_CLASS( dd::BulletComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::BulletComponent, Velocity );
			DD_MEMBER( dd::BulletComponent, HitPosition );
			DD_MEMBER( dd::BulletComponent, Age );
			DD_MEMBER( dd::BulletComponent, Lifetime );
			DD_MEMBER( dd::BulletComponent, PendingHit );
		}
	};
}