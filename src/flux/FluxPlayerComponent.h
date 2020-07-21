//
// FluxPlayerComponent.h
// Copyright (C) Sebastian Nordgren 
// July 21st 2020
//

#pragma once

namespace flux
{
	struct Weapon
	{
		float ShotDelay { 0 };
		float BulletDamage { 0 };
		float BulletSpeed { 0 };

		DD_BEGIN_CLASS(flux::Weapon)
			DD_MEMBER(ShotDelay);
			DD_MEMBER(BulletDamage);
			DD_MEMBER(BulletSpeed);
		DD_END_CLASS()
	};

	struct PlayerStats
	{
		int MaxHealth { 0 };
		float MaxSpeed { 0 };
		float Acceleration { 0 };
		float Deceleration { 0 };

		DD_BEGIN_CLASS(flux::PlayerStats)
			DD_MEMBER(MaxHealth);
			DD_MEMBER(MaxSpeed);
			DD_MEMBER(Acceleration);
			DD_MEMBER(Deceleration);
		DD_END_CLASS()
	};
	
	struct FluxPlayerComponent
	{
		float Health { 0 };
		glm::vec2 Velocity { 0 };

		float ShotCooldown { 0 };

		PlayerStats* Stats { nullptr };
		Weapon* EquippedWeapon { nullptr };

		DD_BEGIN_CLASS(flux::FluxPlayerComponent)
			DD_COMPONENT();

			DD_MEMBER(Health);
			DD_MEMBER(Velocity);

			DD_MEMBER(ShotCooldown);

			DD_MEMBER(Stats);
			DD_MEMBER(EquippedWeapon);
		DD_END_CLASS()
	};
}