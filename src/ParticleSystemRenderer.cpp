//
// ParticleSystemRenderer.cpp - A particle system renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#include "PCH.h"
#include "ParticleSystemRenderer.h"

#include "ICamera.h"

#include "ddr/OpenGL.h"
#include "ddr/RenderData.h"
#include "ddr/ShaderPart.h"
#include "ddr/Shader.h"
#include "ddr/Uniforms.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec2 s_screenFacingQuadVertices[] = {
			glm::vec2(-0.5f,-0.5f),
			glm::vec2(0.5f,	-0.5f),
			glm::vec2(-0.5f,0.5f),
			glm::vec2(-0.5f,0.5f),
			glm::vec2(0.5f,	-0.5f),
			glm::vec2(0.5f,	0.5f)
	};

	static const glm::vec2 s_screenFacingQuadUVs[] = {
		glm::vec2(0,0),
		glm::vec2(1,0),
		glm::vec2(0,1),
		glm::vec2(0,1),
		glm::vec2(1,0),
		glm::vec2(1,1)
	};

	static VBO s_vboQuad;

	static ShaderHandle s_shaderParticle;

	ParticleSystemRenderer::ParticleSystemRenderer() :
		ddr::IRenderer("Particle Systems")
	{
		Require<dd::ParticleSystemComponent>();
		RequireTag(ddc::Tag::Visible);

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void ParticleSystemRenderer::Initialize()
	{
		s_shaderParticle = s_shaderManager->Load("particle");

		if (!s_vboQuad.IsValid())
		{
			s_vboQuad.Create(dd::ConstBuffer<glm::vec2>(s_screenFacingQuadVertices, 6));
		}
	}

	ParticleSystemRenderer::ParticleSystemRenderState ParticleSystemRenderer::GetParticleSystemRenderState(ddc::Entity entity)
	{
		auto it = m_systemRenderStates.find(entity);
		if (it != m_systemRenderStates.end())
		{
			return it->second;
		}

		ParticleSystemRenderState new_state;
		new_state.VAO.Create();

		new_state.Positions.Create(m_positions);
		new_state.VAO.BindVBO(new_state.Positions, 0, sizeof(glm::vec3));
		
		new_state.Sizes.Create(m_sizes);
		new_state.VAO.BindVBO(new_state.Sizes, 0, sizeof(glm::vec3));

		new_state.Colours.Create(m_colours);
		new_state.VAO.BindVBO(new_state.Colours, 0, sizeof(glm::vec3));

		m_systemRenderStates.insert({ entity, new_state });

		return new_state;
	}

	void ParticleSystemRenderer::BindSystemToShader(ParticleSystemRenderer::ParticleSystemRenderState render_state, ScopedShader& shader)
	{
		shader->BindAttributeVec2(render_state.VAO, s_vboQuad, "Position", Normalized::No, Instanced::Yes);
		shader->BindAttributeVec3(render_state.VAO, render_state.Positions, "PositionInstanced", Normalized::No, Instanced::Yes);
		shader->BindAttributeVec2(render_state.VAO, render_state.Sizes, "ScaleInstanced", Normalized::No, Instanced::Yes);
		shader->BindAttributeVec4(render_state.VAO, render_state.Colours, "ColourInstanced");
	}

	void ParticleSystemRenderer::Render(const ddr::RenderData& data)
	{
		ScopedShader shader = s_shaderParticle.Access()->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();

		uniforms.Upload(*shader);
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		auto particle_systems = data.Get<dd::ParticleSystemComponent>();

		const glm::vec3 cam_pos = camera.GetPosition();

		for (int i = 0; i < data.Size(); ++i)
		{
			const dd::ParticleSystemComponent& system = particle_systems[i];
			ParticleSystemRenderState& system_render_state = GetParticleSystemRenderState(data.Entities()[i]);

			std::memcpy(m_tempBuffer, system.Particles, sizeof(dd::Particle) * system.LiveCount);

			for (dd::Particle& p : m_tempBuffer)
			{
				DD_ASSERT(!ddm::IsNaN(p.Position));

				p.Distance = p.Alive() ? glm::distance2(p.Position, cam_pos) : -1;
			}

			std::sort(&m_tempBuffer[0], &m_tempBuffer[dd::MAX_PARTICLES],
				[](const dd::Particle& a, const dd::Particle& b)
				{
					return a.Distance > b.Distance;
				});

			int count = 0;
			for (const dd::Particle& particle : m_tempBuffer)
			{
				if (!particle.Alive())
				{
					break;
				}

				m_positions[count] = particle.Position;
				m_colours[count] = particle.Colour;
				m_sizes[count] = particle.Size;

				++count;
			}

			OpenGL::DrawArraysInstanced(OpenGL::Primitive::Triangles, 6, count);
		}
	}
}
