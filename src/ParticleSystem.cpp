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
#include "TransformComponent.h"

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
		RequireWrite<ddc::ParticleSystemComponent>();
		RequireRead<dd::TransformComponent>();
	}

	ParticleSystem::~ParticleSystem()
	{

	}

	void ParticleSystem::Initialize( ddc::World& world )
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

	ParticleSystemRenderer::ParticleSystemRenderer()
	{
		Require<ddc::ParticleSystemComponent>();
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
		ddc::WriteBuffer<ddc::ParticleSystemComponent> particles = update_data.Write<ddc::ParticleSystemComponent>();
		ddc::ReadBuffer<dd::TransformComponent> transforms = update_data.Read<dd::TransformComponent>();

		for( size_t i = 0; i < particles.Size(); ++i )
		{
			ddc::ParticleSystemComponent& system = particles[ i ];
				
			if( m_killAllParticles )
			{
				for( size_t i = 0; i < ddc::MaxParticles; ++i )
				{
					ddc::Particle& particle = system.m_particles[i];

					if( particle.Alive() )
					{
						particle.Age = particle.Lifetime;
					}
				}

				system.m_age = system.m_lifetime;
			}

			UpdateLiveParticles( system, delta_t );

			if( system.m_age < system.m_lifetime )
			{
				EmitNewParticles( system, transforms[ i ].World, delta_t );
			}
		}
	}

	void ParticleSystem::UpdateLiveParticles( ddc::ParticleSystemComponent& system, float delta_t )
	{
		for( size_t particle_index = 0; particle_index < system.m_liveCount; ++particle_index )
		{
			ddc::Particle& particle = system.m_particles[particle_index];

			if( particle.Alive() )
			{
				particle.Age += float( delta_t );

				if( !particle.Alive() )
				{
					--system.m_liveCount;
					continue;
				}

				float percent_life = particle.Age / particle.Lifetime;
				particle.Colour.a = 1.0f - percent_life;

				particle.Position += particle.Velocity * delta_t;
				particle.Velocity += s_gravity * delta_t;
			}
		}
	}

	void ParticleSystem::EmitNewParticles( ddc::ParticleSystemComponent& system, const glm::mat4& transform, float delta_t )
	{
		system.m_emissionAccumulator += system.m_emissionRate * delta_t;
		int toEmit = (int) system.m_emissionAccumulator;

		system.m_emissionAccumulator = system.m_emissionAccumulator - toEmit;

		int emitted = 0;

		for( int i = 0; i < ddc::MaxParticles; ++i )
		{
			if( emitted >= toEmit || system.m_liveCount > CurrentMaxParticles )
			{
				break;
			}

			ddc::Particle& particle = system.m_particles[ i ];

			if( !particle.Alive() )
			{
				particle.Position = transform[ 3 ].xyz;

				glm::mat3 rotation( transform[ 0 ].xyz, transform[ 1 ].xyz, transform[ 2 ].xyz );

				glm::vec3 velocity = glm::mix( system.m_minVelocity, system.m_maxVelocity, glm::vec3( system.m_rng.Next(), system.m_rng.Next(), system.m_rng.Next() ) );
				particle.Velocity = velocity * rotation;
				particle.Size = glm::mix( system.m_minSize, system.m_maxSize, glm::vec2( system.m_rng.Next(), system.m_rng.Next() ) );
				particle.Lifetime = glm::mix( system.m_minLifetime, system.m_maxLifetime, system.m_rng.Next() );
				particle.Age = 0;
				particle.Colour = glm::vec4( glm::mix( system.m_minColour, system.m_maxColour, glm::vec3( system.m_rng.Next(), system.m_rng.Next(), system.m_rng.Next() ) ), 1 );

				++system.m_liveCount;
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

		ddr::RenderBuffer<ddc::ParticleSystemComponent> particle_systems = data.Get<ddc::ParticleSystemComponent>();

		glm::vec3 cam_pos = camera.GetPosition();

		for( const ddc::ParticleSystemComponent& system : particle_systems )
		{
			memcpy( m_tempBuffer, system.m_particles, sizeof( ddc::Particle ) * ddc::MaxParticles );

			std::sort( &m_tempBuffer[0], &m_tempBuffer[ddc::MaxParticles], 
				[cam_pos]( const ddc::Particle& a, const ddc::Particle& b )
			{
				return glm::distance2( a.Position, cam_pos ) > glm::distance2( b.Position, cam_pos );
			} );

			int index = 0;
			for( const ddc::Particle& particle : m_tempBuffer )
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