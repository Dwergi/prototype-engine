#pragma once

#include "IDebugPanel.h"
#include "SystemsSorting.h"

namespace dd
{
	struct Job;
}

namespace ddc
{
	struct System;
	struct SystemNode;

	struct SystemsManager : dd::IDebugPanel
	{
		//
		// Initialize all currently registered systems.
		//
		void Initialize(EntityLayer& layer);

		//
		// Shut down all currently registered systems.
		//
		void Shutdown(EntityLayer& layer);

		// 
		// Register a system to be updated every frame.
		// 
		void Register(System& system);

		//
		// Update all registered systems that are enabled for the given space with the given delta.
		//
		void Update(EntityLayer& layer, float delta_t);

	private:

		std::vector<System*> m_systems;
		std::vector<SystemNode> m_orderedSystems;

		bool m_drawSystemsGraph { false };

		struct SystemUpdate
		{
			SystemNode* SystemNode { nullptr };
			EntityLayer* Layer { nullptr };
			dd::Job* Dependencies { nullptr };
			float DeltaT { 0 };
		};

		void UpdateSystem(SystemUpdate update);
		void UpdateSystemsWithTreeScheduling(EntityLayer& layer, float delta_t);

		void DrawDebugInternal();

		// Inherited via IDebugPanel
		virtual const char* GetDebugTitle() const override { return "Systems"; }
	};
}