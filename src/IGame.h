//
// IGame.h - Interface for games.
// Copyright (C) Sebastian Nordgren 
// August 2nd 2019
//

#pragma once

namespace ddc
{
	struct EntitySpace;
	struct SystemsManager;
}

namespace ddr
{
	struct ICamera;
	struct RenderManager;
}

namespace dd
{
	struct Input;

	struct GameUpdateData
	{
		GameUpdateData(ddc::EntitySpace& space, dd::Input& input, float delta_t) : m_space(space), m_input(input), m_delta(delta_t) {}

		// Get the entity space.
		ddc::EntitySpace& EntitySpace() const { return m_space; }

		// Get the input system.
		dd::Input& Input() const { return m_input; }
		
		// Get the delta since last frame.
		float Delta() const { return m_delta; }

	private:
		ddc::EntitySpace& m_space;
		dd::Input& m_input;
		float m_delta;
	};

	struct IGame
	{
		// Initialization occurs in this order.
		virtual void Initialize() = 0;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) = 0;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) = 0;
		// Last initialization step - it is valid to populate the entity space here if desired.
		virtual void CreateEntitySpaces(std::vector<ddc::EntitySpace*>& spaces) = 0;

		virtual void Shutdown() = 0;

		virtual void Update(const dd::GameUpdateData& update_data) = 0;

		virtual ddr::ICamera& GetCamera() const = 0;
		virtual const char* GetTitle() const { return "DD"; }
	};
}