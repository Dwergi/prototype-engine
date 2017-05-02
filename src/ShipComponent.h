//
// ShipComponent.h - A ship that is used to implement steering behaviours.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "ComponentBase.h"
#include "DenseMapPool.h"

namespace dd
{
	class ShipComponent : public ComponentBase
	{
	public:
		typedef DenseMapPool<ShipComponent> Pool;

		glm::vec3 Velocity;
		float MaximumSpeed;
		float MinimumSpeed;
		float Acceleration;

		// A multiplier for acceleration when boosting.
		float BoostFactor;
		
		// The amount of time you can boost for, in seconds.
		float BoostRemaining; 

		BEGIN_SCRIPT_OBJECT( ShipComponent )
			PARENT( ComponentBase )
		END_TYPE
	};
};
