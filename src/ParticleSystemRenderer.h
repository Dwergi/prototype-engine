//
// ParticleSystemRenderer.h - A particle system renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#pragma once

#include "ParticleSystemComponent.h"
#include "Renderer.h"
#include "RenderState.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct ParticleSystemRenderer : ddr::Renderer
	{
	public:

		ParticleSystemRenderer();

		virtual void RenderInit( ddc::World& world ) override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render( const ddr::RenderData& data );

	private:
		
		RenderState m_renderState;

		glm::vec3 m_positions[dd::MAX_PARTICLES];
		VBO m_vboPosition;

		glm::vec2 m_sizes[dd::MAX_PARTICLES];
		VBO m_vboSizes;

		glm::vec4 m_colours[dd::MAX_PARTICLES];
		VBO m_vboColours;

		dd::Particle m_tempBuffer[dd::MAX_PARTICLES];
	};
}