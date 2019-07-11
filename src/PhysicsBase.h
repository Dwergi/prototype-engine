//
// PhysicsBase.h
// Copyright (C) Sebastian Nordgren 
// October 8th 2018
//

#pragma once

namespace dd
{
	struct PhysicsBase
	{
		glm::vec3 Momentum;

		float Mass { 1.0f };
		float Elasticity { 1.0f };

		bool Resting { false };

		DD_BEGIN_CLASS( dd::PhysicsBase )
			DD_MEMBER( Momentum );
			DD_MEMBER( Mass );
			DD_MEMBER( Elasticity );
			DD_MEMBER( Resting );
		DD_END_CLASS()
	};
}