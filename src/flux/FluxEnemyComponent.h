//
// FluxEnemyComponent.h
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#pragma once

#include "ddm/Circle.h"

namespace flux
{
	struct FluxEnemyComponent
	{
		float Health { 0 };
		ddm::Circle HitCircle;

		DD_BEGIN_CLASS(flux::FluxEnemyComponent)
			DD_COMPONENT();

			DD_MEMBER(Health);
		DD_END_CLASS()
	};
}