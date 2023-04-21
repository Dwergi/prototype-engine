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
		m_shaderParticle = s_shaderManager->Load("particle");

		m_vboQuad.Create("particle.quad");
		m_vboQuad.SetData(dd::ConstBuffer<glm::vec2>(s_screenFacingQuadVertices, 6));
	}

	ParticleSystemRenderer::ParticleSystemRenderState& ParticleSystemRenderer::GetParticleSystemRenderState(ddc::Entity entity, ddr::Shader& shader)
	{
		auto it = m_systemRenderStates.find(entity);
		if (it != m_systemRenderStates.end())
		{
			return it->second;
		}

		auto it_pair = m_systemRenderStates.insert({ entity, ParticleSystemRenderState() });

		ParticleSystemRenderState& new_state = it_pair.first->second;
		new_state.VAO.Create("particle");

		new_state.VAO.BindVBO(m_vboQuad, 0, sizeof(glm::vec2));
		shader.CreateAttributeVec2(new_state.VAO, m_vboQuad, "Position", Normalized::No, Instanced::Yes);

		new_state.Positions.Create("particle.position");
		new_state.VAO.BindVBO(new_state.Positions, 0, sizeof(glm::vec3));
		shader.CreateAttributeVec3(new_state.VAO, new_state.Positions, "PositionInstanced", Normalized::No, Instanced::Yes);

		new_state.Sizes.Create("particle.scale");
		new_state.VAO.BindVBO(new_state.Sizes, 0, sizeof(glm::vec3));
		shader.CreateAttributeVec2(new_state.VAO, new_state.Sizes, "ScaleInstanced", Normalized::No, Instanced::Yes);

		new_state.Colours.Create("particle.colour");
		new_state.VAO.BindVBO(new_state.Colours, 0, sizeof(glm::vec3));
		shader.CreateAttributeVec4(new_state.VAO, new_state.Colours, "ColourInstanced", Normalized::No, Instanced::No);

		return new_state;
	}

	void ParticleSystemRenderer::Render(const ddr::RenderData& data)
	{
		ScopedShader shader = m_shaderParticle->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();

		uniforms.Upload(*shader);
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		auto particle_systems = data.Get<dd::ParticleSystemComponent>();

		const glm::vec3 cam_pos = camera.GetPosition();

		for (int i = 0; i < data.Size(); ++i)
		{
			const dd::ParticleSystemComponent& system = particle_systems[i];
			ParticleSystemRenderState& render_state = GetParticleSystemRenderState(data.Entities()[i], *shader);

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

			render_state.Positions.SetData(m_positions);
			render_state.Sizes.SetData(m_sizes);
			render_state.Colours.SetData(m_colours);

			render_state.VAO.Bind();

			OpenGL::DrawArraysInstanced(ddr::Primitive::Triangles, 6, count);

			render_state.VAO.Unbind();
		}
	}
}
