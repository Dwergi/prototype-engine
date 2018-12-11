//
// ParticleSystem.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "IHandlesInput.h"
#include "System.h"

namespace dd
{
	struct ICamera;
	struct InputBindings;
	struct ParticleSystemComponent;
	struct TransformComponent;

	struct ParticleSystem : ddc::System, dd::IDebugPanel, dd::IHandlesInput
	{
		ParticleSystem();
		~ParticleSystem();

		virtual void Initialize( ddc::World& ) override;
		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void Shutdown( ddc::World& ) override {}

		virtual void BindActions( dd::InputBindings& input_bindings ) override;

		ParticleSystem( const ParticleSystem& ) = delete;
		ParticleSystem( ParticleSystem&& ) = delete;
		ParticleSystem& operator=( const ParticleSystem& ) = delete;
		ParticleSystem& operator=( ParticleSystem&& ) = delete;

	private:

		typedef uint ParticleID;

		struct SpawnRequest
		{
			ParticleID Particle { 0 };
			glm::vec3 Position;
			glm::vec3 Normal;
		};

		int CurrentMaxParticles { 1000 };

		bool m_killAllParticles { false };
		bool m_startEmitting { false };

		dd::ParticleSystemComponent* m_selected { nullptr };

		std::vector<SpawnRequest> m_pendingSpawns;

		virtual void DrawDebugInternal( ddc::World& world ) override;
		virtual const char* GetDebugTitle() const {	return "Particles"; }

		void UpdateLiveParticles( dd::ParticleSystemComponent& cmp, float delta_t );
		void EmitNewParticles( dd::ParticleSystemComponent& cmp, const dd::TransformComponent& transform, float delta_t );

		void OnBulletHitMessage( dd::Message msg );
	};
}