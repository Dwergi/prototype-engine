//
// ParticleSystem.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IRenderer.h"
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
	struct UniformStorage;

	class ParticleSystem : public dd::IDebugPanel, public ddr::IRenderer, public ddc::System
	{
	public:

		ParticleSystem();
		~ParticleSystem();

		virtual void RenderInit() override;
		virtual bool UsesAlpha() const override { return true; }

		virtual void Update( const ddc::UpdateData& data, float delta_t ) override;
		virtual void Render( const ddr::RenderData& data );

		void BindActions( dd::InputBindings& input_bindings );

		ParticleSystem( const ParticleSystem& ) = delete;
		ParticleSystem( ParticleSystem&& ) = delete;
		ParticleSystem& operator=( const ParticleSystem& ) = delete;
		ParticleSystem& operator=( ParticleSystem&& ) = delete;


	private:

		int CurrentMaxParticles { 1000 };

		bool m_killAllParticles { false };
		bool m_startEmitting { false };

		ddc::ParticleSystemComponent* m_selected { nullptr };

		glm::vec3 m_positions[ ddc::MaxParticles ];
		VBO m_vboPositions;

		glm::vec2 m_sizes[ ddc::MaxParticles ];
		VBO m_vboSizes;

		glm::vec4 m_colours[ ddc::MaxParticles ];
		VBO m_vboColours;

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const {	return "Particles"; }

		void UpdateLiveParticles( ddc::ParticleSystemComponent& cmp, float delta_t );
		void EmitNewParticles( ddc::ParticleSystemComponent& cmp, float delta_t );

		ddc::ReadWriteRequirement<ddc::ParticleSystemComponent> req_particles;
	};
}