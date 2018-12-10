//
// ParticleSystemComponent.h - A particle system component.
// Copyright (C) Sebastian Nordgren 
// August 12th 2018
//

#pragma once

#include "Random.h"
#include "Units.h"

namespace dd
{
	static const int MAX_PARTICLES = 128;

	DD_TODO( "Particles probably don't need to store all of this. Could maybe just parameterize it?" );

	struct Particle
	{
		glm::vec4 Colour;
		glm::vec3 Velocity;
		glm::vec3 Position;
		glm::vec2 Size;
		float Age { 0 };
		float Lifetime { 0 };
		float Distance { -1 };

		bool Alive() const { return Age < Lifetime; }
	};

	struct ParticleSystemComponent 
	{
		ParticleSystemComponent();

		uint Seed { ~0u };

		// in seconds
		float MinLifetime;
		float MaxLifetime;
		
		float Lifetime { 3 };
		float Age { 3 };

		// metres per second
		glm::vec3 MinVelocity; 
		glm::vec3 MaxVelocity;
		
		glm::vec3 MinColour;
		glm::vec3 MaxColour;

		glm::vec2 MinSize;
		glm::vec2 MaxSize;

		size_t LiveCount { 0 };

		float EmissionRate { 200.0 }; // particles per second
		
		// Non-serialized properties
		dd::RandomFloat RNG;

		float EmissionAccumulator { 0 }; // fractional particles that were not emitted last tick

		Particle Particles[MAX_PARTICLES];

		DD_CLASS( dd::ParticleSystemComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::ParticleSystemComponent, Seed );

			DD_MEMBER( dd::ParticleSystemComponent, MinLifetime );
			DD_MEMBER( dd::ParticleSystemComponent, MaxLifetime );

			DD_MEMBER( dd::ParticleSystemComponent, Lifetime );
			DD_MEMBER( dd::ParticleSystemComponent, Age );

			DD_MEMBER( dd::ParticleSystemComponent, MinVelocity );
			DD_MEMBER( dd::ParticleSystemComponent, MaxVelocity );

			DD_MEMBER( dd::ParticleSystemComponent, MinColour );
			DD_MEMBER( dd::ParticleSystemComponent, MaxColour );

			DD_MEMBER( dd::ParticleSystemComponent, MinSize );
			DD_MEMBER( dd::ParticleSystemComponent, MaxSize );

			DD_MEMBER( dd::ParticleSystemComponent, LiveCount );

			DD_MEMBER( dd::ParticleSystemComponent, EmissionRate );
		}
	};
}