//
// ParticleSystem.cpp - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#include "PCH.h"
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
		RequireWrite<dd::ParticleSystemComponent>();
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

	ParticleSystemRenderer::ParticleSystemRenderer() : 
		ddr::Renderer( "Particle Systems" )
	{
		Require<dd::ParticleSystemComponent>();
		RequireTag( ddc::Tag::Visible );
	}

	void ParticleSystemRenderer::RenderInit( ddc::World& world )
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

		shader->Use( false );
	}

	void ParticleSystem::Update( const ddc::UpdateData& update )
	{
		const ddc::DataBuffer& data = update.Data();

		auto particles = data.Write<dd::ParticleSystemComponent>();
		auto transforms = data.Read<dd::TransformComponent>();

		for( size_t i = 0; i < particles.Size(); ++i )
		{
			dd::ParticleSystemComponent& system = particles[ i ];
				
			if( m_killAllParticles )
			{
				for( size_t i = 0; i < dd::MAX_PARTICLES; ++i )
				{
					dd::Particle& particle = system.Particles[i];

					if( particle.Alive() )
					{
						particle.Age = particle.Lifetime;
					}
				}

				system.Age = system.Lifetime;
			}

			UpdateLiveParticles( system, update.Delta() );

			if( system.Age < system.Lifetime )
			{
				EmitNewParticles( system, transforms[ i ].Transform, update.Delta() );
			}
		}
	}

	void ParticleSystem::UpdateLiveParticles( dd::ParticleSystemComponent& system, float delta_t )
	{
		for( size_t particle_index = 0; particle_index < system.LiveCount; ++particle_index )
		{
			dd::Particle& particle = system.Particles[particle_index];

			if( particle.Alive() )
			{
				particle.Age += float( delta_t );

				if( !particle.Alive() )
				{
					--system.LiveCount;
					continue;
				}

				float percent_life = particle.Age / particle.Lifetime;
				particle.Colour.a = 1.0f - percent_life;

				particle.Position += particle.Velocity * delta_t;
				particle.Velocity += s_gravity * delta_t;
			}
		}
	}

	void ParticleSystem::EmitNewParticles( dd::ParticleSystemComponent& system, const glm::mat4& transform, float delta_t )
	{
		system.EmissionAccumulator += system.EmissionRate * delta_t;
		int toEmit = (int) system.EmissionAccumulator;

		system.EmissionAccumulator = system.EmissionAccumulator - toEmit;

		int emitted = 0;

		for( int i = 0; i < dd::MAX_PARTICLES; ++i )
		{
			if( emitted >= toEmit || system.LiveCount > CurrentMAX_PARTICLES )
			{
				break;
			}

			dd::Particle& particle = system.Particles[ i ];

			if( !particle.Alive() )
			{
				particle.Position = transform[ 3 ].xyz;

				glm::mat3 rotation( transform[ 0 ].xyz, transform[ 1 ].xyz, transform[ 2 ].xyz );

				glm::vec3 velocity = glm::mix( system.MinVelocity, system.MaxVelocity, glm::vec3( system.RNG.Next(), system.RNG.Next(), system.RNG.Next() ) );
				particle.Velocity = velocity * rotation;
				particle.Size = glm::mix( system.MinSize, system.MaxSize, glm::vec2( system.RNG.Next(), system.RNG.Next() ) );
				particle.Lifetime = glm::mix( system.MinLifetime, system.MaxLifetime, system.RNG.Next() );
				particle.Age = 0;
				particle.Colour = glm::vec4( glm::mix( system.MinColour, system.MaxColour, glm::vec3( system.RNG.Next(), system.RNG.Next(), system.RNG.Next() ) ), 1 );

				++system.LiveCount;
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
		uniforms.Set( "View", camera.GetViewMatrix() );
		uniforms.Set( "Projection", camera.GetProjectionMatrix() );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		s_vaoParticle.Bind();

		auto particle_systems = data.Get<dd::ParticleSystemComponent>();

		glm::vec3 cam_pos = camera.GetPosition();

		for( const dd::ParticleSystemComponent& system : particle_systems )
		{
			memcpy( m_tempBuffer, system.Particles, sizeof( dd::Particle ) * system.LiveCount );

			for( dd::Particle& p : m_tempBuffer )
			{
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

				m_positions[ index ] = particle.Position;
				m_colours[ index ] = particle.Colour;
				m_sizes[ index ] = particle.Size;

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

		glDisable( GL_BLEND );

		shader->Use( false );
	}

	void ParticleSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SliderInt( "Max Particles", &CurrentMAX_PARTICLES, 0, dd::MAX_PARTICLES );

		if( m_selected == nullptr )
		{
			ImGui::Text( "<no selection>" );
			return;
		}

		if( ImGui::Button( "Start" ) )
		{
			m_selected->Age = 0;
		}

		ImGui::SliderFloat( "Emitter Lifetime", &m_selected->Lifetime, 0, 300 );
		
		{
			float max_emission_rate = CurrentMAX_PARTICLES / m_selected->MaxLifetime; // any higher and we can end up saturating the buffer

			ImGui::SliderFloat( "Emission Rate", &m_selected->EmissionRate, 0.f, max_emission_rate );
		}

		if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::SliderFloat( "Min", &m_selected->MinLifetime, 0, m_selected->MaxLifetime );
			ImGui::SliderFloat( "Max", &m_selected->MaxLifetime, m_selected->MinLifetime, 10 );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Colour", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "R", &m_selected->MinColour.r, &m_selected->MaxColour.r, 0.001f, 0, 1 );
			ImGui::DragFloatRange2( "G", &m_selected->MinColour.g, &m_selected->MaxColour.g, 0.001f, 0, 1 );
			ImGui::DragFloatRange2( "B", &m_selected->MinColour.b, &m_selected->MaxColour.b, 0.001f, 0, 1 );

			ImGui::TreePop();
		}
		
		if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &m_selected->MinVelocity.x, &m_selected->MaxVelocity.x, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Y", &m_selected->MinVelocity.y, &m_selected->MaxVelocity.y, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Z", &m_selected->MinVelocity.z, &m_selected->MaxVelocity.z, 0.1f, -50, 50, "%.1f" );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::DragFloatRange2( "X", &m_selected->MinSize.x, &m_selected->MaxSize.x, 0.1f, -50, 50, "%.1f" );
			ImGui::DragFloatRange2( "Y", &m_selected->MinSize.y, &m_selected->MaxSize.y, 0.1f, -50, 50, "%.1f" );

			ImGui::TreePop();
		}
	}
}