//
// ParticleSystem.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "Renderer.h"
#include "System.h"
#include "ShaderHandle.h"
#include "VAO.h"
#include "VBO.h"
#include "ParticleSystemComponent.h"
#include "DataRequirement.h"
#include "UpdateData.h"

namespace dd
{
	class ICamera;
	class InputBindings;
}

namespace ddc
{
	struct UpdateData;
}

namespace ddr
{
	struct ParticleSystemRenderer : ddr::Renderer
	{
	public:

		ParticleSystemRenderer();

		virtual void RenderInit() override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render( const ddr::RenderData& data );

	private:
		glm::vec3 m_positions[ dd::MaxParticles ];
		VBO m_vboPositions;

		glm::vec2 m_sizes[ dd::MaxParticles ];
		VBO m_vboSizes;

		glm::vec4 m_colours[ dd::MaxParticles ];
		VBO m_vboColours;

		dd::Particle m_tempBuffer[ dd::MaxParticles ];
	};

	struct ParticleSystem : ddc::System, dd::IDebugPanel
	{
	public:

		ParticleSystem();
		~ParticleSystem();

		virtual void Initialize( ddc::World& ) override;
		virtual void Update( const ddc::UpdateData& data, float delta_t ) override;
		virtual void Shutdown( ddc::World& ) override {}

		void BindActions( dd::InputBindings& input_bindings );

		ParticleSystem( const ParticleSystem& ) = delete;
		ParticleSystem( ParticleSystem&& ) = delete;
		ParticleSystem& operator=( const ParticleSystem& ) = delete;
		ParticleSystem& operator=( ParticleSystem&& ) = delete;

	private:

		int CurrentMaxParticles { 1000 };

		bool m_killAllParticles { false };
		bool m_startEmitting { false };

		dd::ParticleSystemComponent* m_selected { nullptr };

		virtual void DrawDebugInternal( const ddc::World& world ) override;
		virtual const char* GetDebugTitle() const {	return "Particles"; }

		void UpdateLiveParticles( dd::ParticleSystemComponent& cmp, float delta_t );
		void EmitNewParticles( dd::ParticleSystemComponent& cmp, const glm::mat4& transform, float delta_t );
	};
}