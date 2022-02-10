//
// ParticleSystemRenderer.h - A particle system renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#pragma once

#include "ParticleSystemComponent.h"
#include "IRenderer.h"

#include "ddr/RenderState.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	struct ParticleSystemRenderer : ddr::IRenderer
	{
	public:

		ParticleSystemRenderer();

		virtual void Initialize() override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render(const ddr::RenderData& data);

	private:
		
		RenderState m_renderState;

		struct ParticleSystemRenderState
		{
			VAO VAO;
			VBO Positions;
			VBO Sizes;
			VBO Colours;
		};

		std::unordered_map<ddc::Entity, ParticleSystemRenderState> m_systemRenderStates;

		glm::vec3 m_positions[dd::MAX_PARTICLES];
		glm::vec2 m_sizes[dd::MAX_PARTICLES];
		glm::vec4 m_colours[dd::MAX_PARTICLES];
		dd::Particle m_tempBuffer[dd::MAX_PARTICLES];

		ParticleSystemRenderState GetParticleSystemRenderState(ddc::Entity entity);
		void BindSystemToShader(ParticleSystemRenderState render_state, struct ScopedShader& shader);
	};
}