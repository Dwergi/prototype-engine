//
// ParticleSystem.cpp - A particle system.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#include "PCH.h"
#include "ParticleSystem.h"

#include "BoundBoxComponent.h"
#include "BulletComponent.h"
#include "ParticleSystemComponent.h"
#include "DataRequest.h"
#include "InputBindings.h"
#include "ParticleSystemComponent.h"
#include "TransformComponent.h"
#include "UpdateData.h"

#include "glm/gtx/norm.hpp"

#include <algorithm>

namespace dd
{
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
		world.Messages().Subscribe( dd::MessageType::BulletHit, [this]( Message msg ) { OnBulletHitMessage( msg ); } );
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

	void ParticleSystem::OnBulletHitMessage( dd::Message msg )
	{
		dd::BulletHitMessage payload = msg.GetPayload<dd::BulletHitMessage>();
		SpawnRequest req;
		req.Position = payload.Position;
		req.Normal = payload.SurfaceNormal;

		m_pendingSpawns.push_back( req );
	}
	
	void ParticleSystem::Update( const ddc::UpdateData& update )
	{
		const ddc::DataBuffer& data = update.Data();
		ddc::World& world = update.World();

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

			system.Age += update.Delta();

			if( system.Age < system.Lifetime )
			{
				EmitNewParticles( system, transforms[ i ].Transform, update.Delta() );
			}
		}

		for( const SpawnRequest& req : m_pendingSpawns )
		{
			ddc::Entity entity = world.CreateEntity<dd::ParticleSystemComponent, dd::TransformComponent, dd::BoundBoxComponent>();
			world.AddTag( entity, ddc::Tag::Visible );

			dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );

			DD_TODO( "I have literally no idea why this works and TransformFromOriginDir doesn't. Fuck matrix math." );

			glm::vec3 up( 0, 1, 0 );
			glm::mat4 lookAt = glm::lookAt( req.Position, req.Position - req.Normal, up );
			lookAt[3].xyz = req.Position;

			DD_ASSERT( !ddm::IsNaN( lookAt[0] ) );

			transform->Transform = lookAt;

			dd::BoundBoxComponent* bounds = world.Access<dd::BoundBoxComponent>( entity );
			bounds->BoundBox = dd::AABB( glm::vec3( -0.5 ), glm::vec3( 0.5 ) );

			dd::ParticleSystemComponent* particle = world.Access<dd::ParticleSystemComponent>( entity );
			particle->Age = 0;
			particle->MinLifetime = 0.1;
			particle->MaxLifetime = 0.2;
			particle->MinColour = glm::vec3( 0.7, 0, 0 );
			particle->MaxColour = glm::vec3( 1.0, 1.0, 0 );
			particle->EmissionRate = 2000;
			particle->MinSize = glm::vec2( 0.1, 0.1 );
			particle->MaxSize = glm::vec2( 0.2, 0.2 );
			particle->MinVelocity = glm::vec3( 0, 0, 5 );
			particle->MaxVelocity = glm::vec3( 5, 5, 7 );
			particle->Lifetime = 0.1;
		}

		m_pendingSpawns.clear();
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
			if( emitted >= toEmit || system.LiveCount > CurrentMaxParticles )
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

				DD_ASSERT( !ddm::IsNaN( particle.Velocity ) );

				particle.Size = glm::mix( system.MinSize, system.MaxSize, glm::vec2( system.RNG.Next(), system.RNG.Next() ) );
				particle.Lifetime = glm::mix( system.MinLifetime, system.MaxLifetime, system.RNG.Next() );
				particle.Age = 0;
				particle.Colour = glm::vec4( glm::mix( system.MinColour, system.MaxColour, glm::vec3( system.RNG.Next(), system.RNG.Next(), system.RNG.Next() ) ), 1 );

				++system.LiveCount;
				++emitted;
			}
		}
	}

	void ParticleSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SliderInt( "Max Particles", &CurrentMaxParticles, 0, dd::MAX_PARTICLES );

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
			float max_emission_rate = CurrentMaxParticles / m_selected->MaxLifetime; // any higher and we can end up saturating the buffer

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