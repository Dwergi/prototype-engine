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
#include "ShaderPart.h"
#include "Shader.h"
#include "Uniforms.h"

namespace ddr
{
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

	static VAO s_vaoParticle;
	static VBO s_vboParticle;
	static ShaderHandle s_shaderParticle;

	ParticleSystemRenderer::ParticleSystemRenderer() :
		ddr::Renderer( "Particle Systems" )
	{
		Require<dd::ParticleSystemComponent>();
		RequireTag( ddc::Tag::Visible );

		m_renderState.BackfaceCulling = false;
		m_renderState.Blending = true;
		m_renderState.Depth = true;
	}

	void ParticleSystemRenderer::RenderInit( ddc::World& world )
	{
		dd::Vector<ShaderPart*> shaders;

		if( !s_shaderParticle.IsValid() )
		{
			s_shaderParticle = ShaderManager::Instance()->Load( "particle" );
		}

		Shader* shader = s_shaderParticle.Access();
		DD_ASSERT( shader != nullptr );

		ScopedShader scoped_state = shader->UseScoped();

		s_vaoParticle.Create();
		s_vaoParticle.Bind();

		s_vboParticle.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboParticle.Bind();
		s_vboParticle.SetData( dd::ConstBuffer<glm::vec2>( s_screenFacingQuadVertices, 6 ) );
		s_vboParticle.CommitData();

		shader->BindAttributeVec2( "Position", false );
		s_vboParticle.Unbind();

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

		s_vaoParticle.Unbind();
	}

	void ParticleSystemRenderer::Render( const ddr::RenderData& data )
	{
		Shader* shader = s_shaderParticle.Access();
		ScopedShader scoped_shader = shader->UseScoped();

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();
		const ddc::World& world = data.World();

		uniforms.Bind( *shader );
		ScopedRenderState scoped_state = m_renderState.UseScoped();

		s_vaoParticle.Bind();

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

			int index = 0;
			for( const dd::Particle& particle : m_tempBuffer )
			{
				if( !particle.Alive() )
				{
					break;
				}

				m_positions[index] = particle.Position;
				m_colours[index] = particle.Colour;
				m_sizes[index] = particle.Size;

				++index;
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

			glDrawArraysInstanced( GL_TRIANGLES, 0, 6, index );
		}

		s_vaoParticle.Unbind();

		uniforms.Unbind();
	}
}