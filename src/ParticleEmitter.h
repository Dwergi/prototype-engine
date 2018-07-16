//
// ParticleEmitter.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"

namespace dd
{
	class ICamera;
}

namespace ddr
{
	class UniformStorage;

	struct Particle
	{
		glm::vec4 Colour;
		glm::vec3 Velocity;
		glm::vec3 Position;
		glm::vec2 Size;
		float Age { 0 };
		float Lifetime { 0 };
		bool Alive { false };
	};

	class ParticleEmitter
	{
	public:

		ParticleEmitter();
		~ParticleEmitter();

		void Update( float delta_t );
		void Render( const dd::ICamera& camera, UniformStorage& uniforms );

		static void CreateRenderResources();

		ParticleEmitter( const ParticleEmitter& ) = delete;
		ParticleEmitter( ParticleEmitter&& ) = delete;
		ParticleEmitter& operator=( const ParticleEmitter& ) = delete;
		ParticleEmitter& operator=( ParticleEmitter&& ) = delete;

	private:

		static const int MaxParticles = 512;
		Particle m_particles[ MaxParticles ];

		int m_liveCount { 0 };

		float m_age { 0 };
		float m_emissionRate { 0 }; // particles per second
	};
}