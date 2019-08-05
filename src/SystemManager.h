#pragma once

namespace ddc
{
	struct System;
	struct SystemNode;

	struct SystemManager
	{
		//
		// Initialize all currently registered systems.
		//
		void Initialize();

		//
		// Shut down all currently registered systems.
		//
		void Shutdown();

		// 
		// Register a system to be updated every frame.
		// 
		void Register(System& system);

		//
		// Update all registered systems with the given delta.
		//
		void Update(float delta_t);

	private:

		std::vector<System*> m_systems;
		std::vector<SystemNode> m_orderedSystems;

		bool m_drawSystemsGraph { false };

		void UpdateSystem(System* system, std::vector<std::shared_future<void>> dependencies, float delta_t);
		void UpdateSystemsWithTreeScheduling(float delta_t);
	};
}