//
// IGame.h - Interface for games.
// Copyright (C) Sebastian Nordgren 
// August 2nd 2019
//

#pragma once

namespace ddc
{
	struct EntityLayer;
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
		GameUpdateData(ddc::EntityLayer& layer, dd::Input& input, float delta_t) : m_layer(layer), m_input(input), m_delta(delta_t) {}

		// Get the entity space.
		ddc::EntityLayer& EntityLayer() const { return m_layer; }

		// Get the input system.
		dd::Input& Input() const { return m_input; }
		
		// Get the delta since last frame.
		float Delta() const { return m_delta; }

	private:
		ddc::EntityLayer& m_layer;
		dd::Input& m_input;
		float m_delta;
	};

	struct IGame
	{
		// Initialization occurs in this order.
		virtual void Initialize() = 0;
		virtual void RegisterSystems(ddc::SystemsManager& system_manager) = 0;
		virtual void RegisterRenderers(ddr::RenderManager& renderer) = 0;
		// Last initialization step - it is valid to populate the entity layer here if desired.
		virtual void CreateEntityLayers(std::vector<ddc::EntityLayer*>& layers) = 0;

		virtual void Shutdown() = 0;

		virtual void Update(const dd::GameUpdateData& update_data) = 0;

		virtual ddr::ICamera& GetCamera() const = 0;
		virtual const char* GetTitle() const { return "DD"; }
	};
}