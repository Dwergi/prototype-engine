//
// ParticleSystemRenderer.cpp - A particle system renderer.
// Copyright (C) Sebastian Nordgren 
// October 3rd 2018
//

#include "PCH.h"
#include "ParticleSystemRenderer.h"

#include "ICamera.h"
#include "OpenGL.h"
#include "RenderData.h"
#include "Services.h"
#include "ShaderPart.h"
#include "Shader.h"
#include "Uniforms.h"

namespace ddr
{
	static dd::Service<ddr::ShaderManager> s_shaderManager;

	static const glm::vec2 s_screenFacingQuadVertices[] = {
			glm::vec2( -0.5f,	-0.5f ),
			glm::vec2( 0.5f,	-0.5f ),
			glm::vec2( -0.5f,	0.5f ),
			glm::vec2( -0.5f,	0.5f ),
			glm::vec2( 0.5f,	-0.5f ),
			glm::vec2( 0.5f,	0.5f )
	};

	static const glm::vec2 s_screenFacingQuadUVs[] = {
		glm::vec2( 0,	0 ),
		glm::vec2( 1,	0 ),
		glm::vec2( 0,	1 ),
		glm::vec2( 0,	1 ),
		glm::vec2( 1,	0 ),
		glm::vec2( 1,	1 )
	};

	static VBO s_vboQuad;
	
	static ShaderHandle s_shaderParticle;

	ParticleSystemRenderer::ParticleSystemRenderer() :
		ddr::IRenderer( "Particle Systems" )
	{
		Require<dd::ParticleSystemComponent>();
		RequireTag( ddc::Tag::Visible );

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void ParticleSystemRenderer::RenderInit( ddc::EntitySpace& entities )
	{
		s_shaderParticle = s_shaderManager->Load( "particle" );

		Shader* shader = s_shaderParticle.Access();
		DD_ASSERT( shader != nullptr );

		ScopedShader scoped_state = shader->UseScoped();

		m_vaoParticle.Create();
		m_vaoParticle.Bind();

		s_vboQuad.Bind();

		if (!s_vboQuad.IsValid())
		{
			s_vboQuad.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
			s_vboQuad.SetData(dd::ConstBuffer<glm::vec2>(s_screenFacingQuadVertices, 6));
			s_vboQuad.CommitData();
		}

		shader->BindAttributeVec2( "Position", false );
		s_vboQuad.Unbind();

		m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboPosition.Bind();
		m_vboPosition.SetData( dd::ConstBuffer<glm::vec3>( m_positions, dd::MAX_PARTICLES ) );
		m_vboPosition.CommitData();

		shader->BindAttributeVec3( "PositionInstanced", false );
		shader->SetAttributeInstanced( "PositionInstanced" );
		m_vboPosition.Unbind();

		m_vboSizes.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboSizes.Bind();
		m_vboSizes.SetData( dd::ConstBuffer<glm::vec2>( m_sizes, dd::MAX_PARTICLES ) );
		m_vboSizes.CommitData();

		shader->BindAttributeVec2( "ScaleInstanced", false );
		shader->SetAttributeInstanced( "ScaleInstanced" );
		m_vboSizes.Unbind();

		m_vboColours.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboColours.Bind();
		m_vboColours.SetData( dd::ConstBuffer<glm::vec4>( m_colours, dd::MAX_PARTICLES ) );
		m_vboColours.CommitData();

		shader->BindAttributeVec4( "ColourInstanced", false );
		shader->SetAttributeInstanced( "ColourInstanced" );
		m_vboColours.Unbind();

		m_vaoParticle.Unbind();
	}

	void ParticleSystemRenderer::Render( const ddr::RenderData& data )
	{
		Shader* shader = s_shaderParticle.Access();
		ScopedShader scoped_shader = shader->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();
		const ddc::EntitySpace& entities = data.EntitySpace();

		uniforms.Bind( *shader );
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		m_vaoParticle.Bind();

		auto particle_systems = data.Get<dd::ParticleSystemComponent>();

		glm::vec3 cam_pos = camera.GetPosition();

		for( const dd::ParticleSystemComponent& system : particle_systems )
		{
			memcpy( m_tempBuffer, system.Particles, sizeof( dd::Particle ) * system.LiveCount );

			for( dd::Particle& p : m_tempBuffer )
			{
				DD_ASSERT( !ddm::IsNaN( p.Position ) );
				
				p.Distance = p.Alive() ? glm::distance2( p.Position, cam_pos ) : -1;
			}

			std::sort( &m_tempBuffer[0], &m_tempBuffer[dd::MAX_PARTICLES],
				[]( const dd::Particle& a, const dd::Particle& b )
			{
				return a.Distance > b.Distance;
			} );

			int count = 0;
			for( const dd::Particle& particle : m_tempBuffer )
			{
				if( !particle.Alive() )
				{
					break;
				}

				m_positions[count] = particle.Position;
				m_colours[count] = particle.Colour;
				m_sizes[count] = particle.Size;

				++count;
			}

			m_vboPosition.Bind();
			m_vboPosition.CommitData();
			m_vboPosition.Unbind();

			m_vboSizes.Bind();
			m_vboSizes.CommitData();
			m_vboSizes.Unbind();

			m_vboColours.Bind();
			m_vboColours.CommitData();
			m_vboColours.Unbind();

			OpenGL::DrawArraysInstanced(m_vboPosition.GetDataSize(), count);
		}

		m_vaoParticle.Unbind();

		uniforms.Unbind();
	}
}