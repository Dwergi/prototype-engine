//
// PhysicsPlaneComponent.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "HitResult.h"

namespace neut
{
	struct HitState;

	struct BulletHitMessage
	{
		glm::vec3 Position;
		glm::vec3 SurfaceNormal;
		glm::vec3 Velocity;

		DD_BEGIN_CLASS( neut::BulletHitMessage )
			DD_MEMBER( Position );
			DD_MEMBER( SurfaceNormal );
			DD_MEMBER( Velocity );
		DD_END_CLASS()
	};

	struct BulletComponent
	{
		glm::vec3 Velocity;
		glm::vec3 HitPosition;
		glm::vec3 HitNormal;
		
		float Age { 0.0f };
		float Lifetime { 10.0f };
		
		dd::HitHandle PendingHit; 
		
		DD_BEGIN_CLASS( neut::BulletComponent )
			DD_COMPONENT();

			DD_MEMBER( Velocity );
			DD_MEMBER( HitPosition );
			DD_MEMBER( Age );
			DD_MEMBER( Lifetime );
			DD_MEMBER( PendingHit );
		DD_END_CLASS()
	};
}