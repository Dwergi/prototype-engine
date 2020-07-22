//
// FluxBulletComponent.h
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#pragma once

namespace flux
{
	enum class BulletType
	{
		Friendly,
		Enemy
	};

	struct FluxBulletComponent
	{
		glm::vec2 Velocity;
		float Damage;
		BulletType Type;

		DD_BEGIN_CLASS(flux::FluxBulletComponent)
			DD_COMPONENT();

			DD_MEMBER(Velocity);
			DD_MEMBER(Damage);
			DD_MEMBER(Type);
		DD_END_CLASS()
	};
}

DD_ENUM(flux::BulletType)
