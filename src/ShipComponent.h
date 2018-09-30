//
// ShipComponent.h - A ship that is used to implement steering behaviours.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once



namespace dd
{
	class ShipComponent
	{
	public:
		glm::vec3 Velocity;
		float MaximumSpeed { 0 };
		float MinimumSpeed { 0 };
		float Acceleration { 0 };

		// A multiplier for acceleration when boosting.
		float BoostFactor { 0 };
		
		// The amount of time you can boost for, in seconds.
		float BoostRemaining { 0 };

		// The number of seconds it takes to recharge one second of boost.
		float BoostRechargeRate { 0 };

		// The number of seconds to recharge to full boost.
		float BoostMaximum { 0 };

		DD_CLASS( dd::ShipComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::ShipComponent, Velocity );
			DD_MEMBER( dd::ShipComponent, MaximumSpeed );
			DD_MEMBER( dd::ShipComponent, MinimumSpeed );
			DD_MEMBER( dd::ShipComponent, Acceleration );
			DD_MEMBER( dd::ShipComponent, BoostFactor );
			DD_MEMBER( dd::ShipComponent, BoostRemaining );
			DD_MEMBER( dd::ShipComponent, BoostRechargeRate );
			DD_MEMBER( dd::ShipComponent, BoostMaximum );
		}
	};
};
