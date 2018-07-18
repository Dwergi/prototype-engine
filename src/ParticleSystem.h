//
// ParticleSystem.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "Random.h"
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
		float Distance { -1 };

		bool Alive() { return Age < Lifetime; }
	};

	class ParticleSystem : public dd::IDebugPanel, public dd::IRenderer, public dd::ISystem
	{
	public:

		ParticleSystem();
		~ParticleSystem();

		virtual void RenderInit() override;
		virtual bool UsesAlpha() const override { return true; }

		virtual void Update( dd::EntityManager& entity_manager, float delta_t ) override;
		virtual void Render( const dd::EntityManager& entity_manager, const dd::ICamera& camera, UniformStorage& uniforms ) override;

		ParticleSystem( const ParticleSystem& ) = delete;
		ParticleSystem( ParticleSystem&& ) = delete;
		ParticleSystem& operator=( const ParticleSystem& ) = delete;
		ParticleSystem& operator=( ParticleSystem&& ) = delete;

	private:

		static const int MaxParticles = 10000;
		static int CurrentParticles;

		static dd::RandomFloat s_rngLifetime;

		static dd::RandomFloat s_rngVelocityX;
		static dd::RandomFloat s_rngVelocityY;
		static dd::RandomFloat s_rngVelocityZ;

		static dd::RandomFloat s_rngColourR;
		static dd::RandomFloat s_rngColourG;
		static dd::RandomFloat s_rngColourB;

		static dd::RandomFloat s_rngSizeX;
		static dd::RandomFloat s_rngSizeY;

		Particle m_particles[ MaxParticles ];

		int m_liveCount { 0 };

		float m_age { 0 };			// in seconds
		float m_emissionRate { 0 }; // particles per second

		glm::vec3 m_positions[ MaxParticles ];
		VBO m_vboPositions;

		glm::vec2 m_sizes[ MaxParticles ];
		VBO m_vboSizes;

		glm::vec4 m_colours[ MaxParticles ];
		VBO m_vboColours;

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const {	return "Particles"; }
	};
}