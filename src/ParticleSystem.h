//
// ParticleSystem.h - A particle emitter.
// Copyright (C) Sebastian Nordgren 
// July 16th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "MessageQueue.h"
#include "System.h"

namespace dd
{
	struct InputKeyBindings;
	struct ParticleSystemComponent;
	struct TransformComponent;

	struct ParticleSystem : ddc::System, dd::IDebugPanel
	{
		ParticleSystem();
		~ParticleSystem();

		virtual void Initialize( ddc::EntityLayer& ) override;
		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void Shutdown( ddc::EntityLayer& ) override {}

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

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const {	return "Particles"; }

		void UpdateLiveParticles( dd::ParticleSystemComponent& cmp, float delta_t );
		void EmitNewParticles( dd::ParticleSystemComponent& cmp, const dd::TransformComponent& transform, float delta_t );

		void OnBulletHitMessage( ddc::Message msg );
	};
}