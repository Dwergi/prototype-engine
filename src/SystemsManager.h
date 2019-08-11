#pragma once

#include "IDebugPanel.h"
#include "SystemsSorting.h"

namespace ddc
{
	struct System;
	struct SystemNode;

	struct SystemsManager : dd::IDebugPanel
	{
		//
		// Initialize all currently registered systems.
		//
		void Initialize(EntitySpace& space);

		//
		// Shut down all currently registered systems.
		//
		void Shutdown(EntitySpace& space);

		// 
		// Register a system to be updated every frame.
		// 
		void Register(System& system);

		//
		// Update all registered systems that are enabled for the given space with the given delta.
		//
		void Update(EntitySpace& space, float delta_t);

	private:

		std::vector<System*> m_systems;
		std::vector<SystemNode> m_orderedSystems;

		bool m_drawSystemsGraph { false };

		void UpdateSystem(System& system, EntitySpace& space, std::vector<std::shared_future<void>> dependencies, float delta_t);
		void UpdateSystemsWithTreeScheduling(EntitySpace& space, float delta_t);

		void DrawDebugInternal();

		// Inherited via IDebugPanel
		virtual const char* GetDebugTitle() const override { return "Systems"; }
	};
}