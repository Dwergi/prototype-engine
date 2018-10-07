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
		glm::vec3 Velocity;
		glm::vec3 Acceleration;
		glm::vec3 Momentum;

		float Mass { 1.0f };
		float Elasticity { 1.0f };

		bool Resting { false };

		DD_CLASS( dd::PhysicsBase )
		{
			DD_MEMBER( dd::PhysicsBase, Velocity );
			DD_MEMBER( dd::PhysicsBase, Acceleration );
			DD_MEMBER( dd::PhysicsBase, Momentum );
			DD_MEMBER( dd::PhysicsBase, Mass );
			DD_MEMBER( dd::PhysicsBase, Elasticity );
			DD_MEMBER( dd::PhysicsBase, Resting );
		}
	};
}