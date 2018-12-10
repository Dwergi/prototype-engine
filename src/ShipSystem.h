//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "IDebugPanel.h"
#include "IHandlesInput.h"
#include "InputAction.h"
#include "Mesh.h"
#include "System.h"

namespace dd
{
	struct InputBindings;
	class ShipComponent;
	struct TransformComponent;

	struct ShipSystem : ddc::System, IDebugPanel, IHandlesInput
	{
		ShipSystem();
		ShipSystem( const ShipSystem& other ) = delete;

		virtual void BindActions( InputBindings& bindings ) override;

		virtual void Initialize( ddc::World& world ) override {}
		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void Shutdown( ddc::World& world ) override;

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Ship"; }

	private:

		glm::vec3 m_nextCameraPos;

		ddc::Entity m_lastShip;
		ddr::MeshHandle m_shipMesh;

		DenseMap<InputAction, bool> m_inputs;
		bool m_enabled { false };
	
		void HandleInput( InputAction action, InputType type );
		void UpdateShip( TransformComponent& transform, ShipComponent& ship, float delta_t );

		void CreateShip( ddc::World& world );

		virtual void DrawDebugInternal( ddc::World& world ) override;
	};
}
