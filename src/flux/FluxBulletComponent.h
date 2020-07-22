//
// FluxBulletComponent.h
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#pragma once

#include "ddm/Circle.h"

namespace flux
{
	enum class BulletType
	{
		Friendly,
		Enemy
	};

	struct FluxBulletComponent
	{
		glm::vec2 Velocity { 0, 0 };
		float Damage { 0 };
		BulletType Type { BulletType::Friendly };
		float Lifetime { 0 };

		ddm::Circle HitCircle;

		DD_BEGIN_CLASS(flux::FluxBulletComponent)
			DD_COMPONENT();

			DD_MEMBER(Velocity);
			DD_MEMBER(Damage);
			DD_MEMBER(Type);
		DD_END_CLASS()
	};
}

DD_ENUM(flux::BulletType)
