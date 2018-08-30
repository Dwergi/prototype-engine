//
// ParticleSystem.cpp - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#include "PrecompiledHeader.h"
#include "ParticleSystem.h"

#include "ICamera.h"
#include "InputBindings.h"
#include "OpenGL.h"
#include "RenderData.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Uniforms.h"

#include "glm/gtx/norm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "GL/gl3w.h"

#include "imgui/imgui.h"

#include <algorithm>

namespace ddr
{
	static const glm::vec2 s_screenFacingQuadVertices[] = {
		glm::vec2( -0.5f,	-0.5f	),
		glm::vec2( 0.5f,	-0.5f	),
		glm::vec2( -0.5f,	0.5f	),
		glm::vec2( -0.5f,	0.5f	),
		glm::vec2( 0.5f,	-0.5f	),
		glm::vec2( 0.5f,	0.5f	)
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

	static const glm::vec3 s_gravity( 0, -9.81, 0 );

	ParticleSystem::ParticleSystem() :
		ddc::System( "Particles" )
	{
		RequireRead<ddc::ParticleSystemComponent>();
	}

	ParticleSystem::~ParticleSystem()
	{

	}
	
	void ParticleSystem::BindActions( dd::InputBindings& bindings )
	{
		bindings.RegisterHandler( dd::InputAction::START_PARTICLE, [this]( dd::InputAction action, dd::InputType type )
		{
			if( type == dd::InputType::RELEASED )
			{
				m_startEmitting = true;
			}
		} );
	}

	void ParticleSystemRenderer::RenderInit()
	{
		dd::Vector<Shader*> shaders;

		if( !s_shaderParticle.Valid() )
		{
			s_shaderParticle = ShaderProgram::Load( "particle" );
		}

		ShaderProgram* shader = ShaderProgram::Get( s_shaderParticle );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		s_vaoParticle.Create();
		s_vaoParticle.Bind();

		s_vboParticle.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		s_vboParticle.Bind();
		s_vboParticle.SetData( dd::ConstBuffer<glm::vec2>( s_screenFacingQuadVertices, 6 ) );
		shader->BindAttributeVec2( "Position", false );
		s_vboParticle.Unbind();

		m_vboPositions.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboPositions.Bind();
		m_vboPositions.SetData( dd::ConstBuffer<glm::vec3>( m_positions, ddc::MaxParticles ) );
		shader->BindAttributeVec3( "PositionInstanced", false );
		shader->SetAttributeInstanced( "PositionInstanced" );
		m_vboPositions.Unbind();

		m_vboSizes.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboSizes.Bind();
		m_vboSizes.SetData( dd::ConstBuffer<glm::vec2>( m_sizes, ddc::MaxParticles ) );
		shader->BindAttributeVec2( "ScaleInstanced", false );
		shader->SetAttributeInstanced( "ScaleInstanced" );
		m_vboSizes.Unbind();

		m_vboColours.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vboColours.Bind();
		m_vboColours.SetData( dd::ConstBuffer<glm::vec4>( m_colours, ddc::MaxParticles ) );
		shader->BindAttributeVec4( "ColourInstanced", false );
		shader->SetAttributeInstanced( "ColourInstanced" );
		m_vboColours.Unbind();

		s_vaoParticle.Unbind();

		shader->Use( false );
	}

	void ParticleSystem::Update( const ddc::UpdateData& update_data, float delta_t )
	{
		ddc::WriteBuffer<ddc::ParticleSystemComponent> data_buffer = update_data.Write<ddc::ParticleSystemComponent>();

		for( size_t i = 0; i < data_buffer.Size(); ++i )
		{
			ddc::ParticleSystemComponent& cmp = data_buffer.Access( i );

			if( m_killAllParticles )
			{
				for( size_t i = 0; i < ddc::MaxParticles; ++i )
				{
					ddc::Particle& particle = cmp.m_particles[i];

					if( particle.Alive() )
					{
						particle.Age = particle.Lifetime;
					}
				}

				cmp.m_age = cmp.m_lifetime;
			}

			UpdateLiveParticles( cmp, delta_t );

			if( cmp.m_age < cmp.m_lifetime )
			{
				EmitNewParticles( cmp, delta_t );
			}
		}
	}

	void ParticleSystem::UpdateLiveParticles( ddc::ParticleSystemComponent& cmp, float delta_t )
	{
		for( size_t particle_index = 0; particle_index < cmp.m_liveCount; ++particle_index )
		{
			ddc::Particle& particle = cmp.m_particles[particle_index];

			if( particle.Alive() )
			{
				particle.Age += float( delta_t );

				if( !particle.Alive() )
				{
					--cmp.m_liveCount;
					continue;
				}

				float percent_life = particle.Age / particle.Lifetime;
				particle.Colour.a = 1.0f - percent_life;

				particle.Position += particle.Velocity * delta_t;
				particle.Velocity += s_gravity * delta_t;
			}
		}
	}

	void ParticleSystem::EmitNewParticles( ddc::ParticleSystemComponent& cmp, float delta_t )
	{
		cmp.m_emissionAccumulator += cmp.m_emissionRate * delta_t;
		int toEmit = (int) cmp.m_emissionAccumulator;

		cmp.m_emissionAccumulator = cmp.m_emissionAccumulator - toEmit;

		int emitted = 0;

		for( int i = 0; i < ddc::MaxParticles; ++i )
		{
			if( emitted >= toEmit || cmp.m_liveCount > CurrentMaxParticles )
			{
				break;
			}

			ddc::Particle& particle = cmp.m_particles[ i ];

			if( !particle.Alive() )
			{
				particle.Position = glm::vec3( 0, 50, 0 );
				particle.Velocity = glm::mix( cmp.m_minVelocity, cmp.m_maxVelocity, glm::vec3( cmp.m_rng.Next(), cmp.m_rng.Next(), cmp.m_rng.Next() ) );
				particle.Size = glm::mix( cmp.m_minSize, cmp.m_maxSize, glm::vec2( cmp.m_rng.Next(), cmp.m_rng.Next() ) );
				particle.Lifetime = glm::mix( cmp.m_minLifetime, cmp.m_maxLifetime, cmp.m_rng.Next() );
				particle.Age = 0;
				particle.Colour = glm::vec4( glm::mix( cmp.m_minColour, cmp.m_maxColour, glm::vec3( cmp.m_rng.Next(), cmp.m_rng.Next(), cmp.m_rng.Next() ) ), 1 );

				++cmp.m_liveCount;
				++emitted;
			}
		}
	}

	void ParticleSystemRenderer::Render( const ddr::RenderData& data )
	{
		ShaderProgram* shader = ShaderProgram::Get( s_shaderParticle );
		shader->Use( true );

		ddr::UniformStorage& uniforms = data.Uniforms();
		const ddr::ICamera& camera = data.Camera();
		const ddc::World& world = data.World();

		uniforms.Bind( *shader );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		s_vaoParticle.Bind();

		std::vector<ddc::Entity> particle_systems;
		world.FindAllWith<ddc::ParticleSystemComponent>( particle_systems );

		for( ddc::Entity& entity : particle_systems )
		{
			ddc::ParticleSystemComponent* cmp = world.AccessComponent<ddc::ParticleSystemComponent>( entity );
			for( ddc::Particle& particle : cmp->m_particles )
			{
				if( particle.Alive() )
				{
					particle.Distance = glm::distance2( particle.Position, camera.GetPosition() );
				}
				else
				{
					particle.Distance = -1;
				}
			}

			std::sort( &cmp->m_particles[0], &cmp->m_particles[ddc::MaxParticles], []( const ddc::Particle& a, const ddc::Particle& b )
			{
				return a.Distance > b.Distance;
			} );

			int index = 0;
			for( ddc::Particle& particle : cmp->m_particles )
			{
				if( particle.Alive() )
				{
					m_positions[index] = particle.Position;
					m_colours[index] = particle.Colour;
					m_sizes[index] = particle.Size;

					++index;
				}
			}

			m_vboPositions.UpdateData();
			m_vboSizes.UpdateData();
			m_vboColours.UpdateData();

			glDrawArraysInstanced( GL_TRIANGLES, 0, 6, index );
		}

		s_vaoParticle.Unbind();

		glDisable( GL_BLEND );

		shader->Use( false );
	}

	void ParticleSystem::DrawDebugInternal()
	{
		ImGui::SliderInt( "Max Particles", &CurrentMaxParticles, 0, ddc::MaxParticles );

		if( m_selected == nullptr )
		{
			ImGui::Text( "<no selection>" );
			return;
		}

		if( ImGui::Button( "Start" ) )
		{
			m_selected->m_age = 0;
		}

		ImGui::SliderFloat( "Emitter Lifetime", &m_selected->m_lifetime, 0, 300 );
		
		{
			float max_emission_rate = CurrentMaxParticles / m_selected->m_maxLifetime; // any higher and we can end up saturating the buffer

			ImGui::SliderFloat( "Emission Rate", &m_selected->m_emissionRate, 0.f, max_emission_rate );
		}

		if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::SliderFloat( "Min", &m_selected->m_minLifetime, 0, m_selected->m_maxLifetime );
			ImGui::SliderFloat( "Max", &m_selected->m_maxLifetime, m_selected->m_minLifetime, 10 );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Colour", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "R", &m_selected->m_minColour.r, &m_selected->m_maxColour.r, 0.001f, 0, 1 );
			ImGui::DragFloatRange2( "G", &m_selected->m_minColour.g, &m_selected->m_maxColour.g, 0.001f, 0, 1 );
			ImGui::DragFloatRange2( "B", &m_selected->m_minColour.b, &m_selected->m_maxColour.b, 0.001f, 0, 1 );

			ImGui::TreePop();
		}
		
		if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &m_selected->m_minVelocity.x, &m_selected->m_maxVelocity.x, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Y", &m_selected->m_minVelocity.y, &m_selected->m_maxVelocity.y, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Z", &m_selected->m_minVelocity.z, &m_selected->m_maxVelocity.z, 0.1f, -50, 50, "%.1f" );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &m_selected->m_minSize.x, &m_selected->m_maxSize.x, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Y", &m_selected->m_minSize.y, &m_selected->m_maxSize.y, 0.1f, -50, 50, "%.1f" );

			ImGui::TreePop();
		}
	}
}