//
// ParticleSystemComponent.h - A particle system component.
// Copyright (C) Sebastian Nordgren 
// August 12th 2018
//

#pragma once

#include "ComponentType.h"
#include "Random.h"
#include "Units.h"

namespace ddc
{
	static const int MaxParticles = 8 * 1024;

	struct Particle
	{
		glm::vec4 Colour;
		glm::vec3 Velocity;
		glm::vec3 Position;
		glm::vec2 Size;
		float Age { 0 };
		float Lifetime { 0 };
		dd::metres Distance { -1 };

		bool Alive() const { return Age < Lifetime; }
	};

	struct ParticleSystemComponent 
	{
		DD_COMPONENT;

		ParticleSystemComponent();

		uint m_seed { ~0u };
		dd::RandomFloat m_rng;

		// in seconds
		float m_minLifetime;
		float m_maxLifetime;
		float m_lifetime { 3 };

		// metres per second
		glm::vec3 m_minVelocity; 
		glm::vec3 m_maxVelocity;
		
		glm::vec3 m_minColour;
		glm::vec3 m_maxColour;

		glm::vec2 m_maxSize;
		glm::vec2 m_minSize;

		size_t m_liveCount { 0 };
		Particle m_particles[MaxParticles];

		float m_age { 3 };	// in seconds

		float m_emissionRate { 200.0 }; // particles per second
		float m_emissionAccumulator { 0 }; // fractional particles that were not emitted last tick
	};
}