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
		glm::vec3 Normal;
		glm::vec3 Velocity;
		ddc::Entity HitEntity;

		DD_BEGIN_CLASS(neut::BulletHitMessage)
			DD_MEMBER(Position);
			DD_MEMBER(Normal);
			DD_MEMBER(Velocity);
			DD_MEMBER(HitEntity);
		DD_END_CLASS()
	};

	struct BulletComponent
	{
		glm::vec3 Velocity;
		float Age { 0.0f };
		float Lifetime { 10.0f };

		dd::HitHandle PendingHit;
		dd::HitResult HitResult;

		DD_BEGIN_CLASS(neut::BulletComponent)
			DD_COMPONENT();

			DD_MEMBER(Velocity);
			DD_MEMBER(Age);
			DD_MEMBER(Lifetime);
			DD_MEMBER(PendingHit);
			DD_MEMBER(HitResult);
		DD_END_CLASS()
	};
}