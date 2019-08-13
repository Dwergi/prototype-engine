//
// ParticleSystemRenderer.h - A particle system renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#pragma once

#include "ParticleSystemComponent.h"
#include "IRenderer.h"
#include "RenderState.h"
#include "VAO.h"
#include "VBO.h"

namespace ddr
{
	struct ParticleSystemRenderer : ddr::IRenderer
	{
	public:

		ParticleSystemRenderer();

		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual bool UsesAlpha() const override { return true; }
		virtual void Render( const ddr::RenderData& data );

	private:
		
		RenderState m_renderState;

		VAO m_vaoParticle;

		glm::vec3 m_positions[dd::MAX_PARTICLES];
		VBO m_vboPosition;

		glm::vec2 m_sizes[dd::MAX_PARTICLES];
		VBO m_vboSizes;

		glm::vec4 m_colours[dd::MAX_PARTICLES];
		VBO m_vboColours;

		dd::Particle m_tempBuffer[dd::MAX_PARTICLES];
	};
}