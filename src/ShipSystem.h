//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "MeshHandle.h"
#include "IDebugPanel.h"
#include "InputAction.h"
#include "System.h"

namespace dd
{
	class FPSCamera;
	struct InputBindings;
	class ShipComponent;
	struct TransformComponent;

	class ShipSystem : public ddc::System, public IDebugPanel
	{
	public:

		ShipSystem( FPSCamera& camera );
		ShipSystem( const ShipSystem& other ) = delete;

		void BindActions( InputBindings& bindings );

		virtual void Initialize( ddc::World& world ) override {}
		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void Shutdown( ddc::World& world ) override;

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Ship"; }

	private:

		FPSCamera& m_camera;
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
