//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "IDebugPanel.h"
#include "InputAction.h"
#include "Mesh.h"
#include "System.h"

namespace dd
{
	struct TransformComponent;
}

namespace neut
{
	struct ShipComponent;

	struct ShipSystem : ddc::System, dd::IDebugPanel
	{
		ShipSystem();
		ShipSystem( const ShipSystem& other ) = delete;

		virtual void Initialize( ddc::EntitySpace& entities ) override {}
		virtual void Update( const ddc::UpdateData& data ) override;
		virtual void Shutdown( ddc::EntitySpace& entities ) override;

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Ship"; }

	private:

		glm::vec3 m_nextCameraPos;

		ddc::Entity m_lastShip;
		ddr::MeshHandle m_shipMesh;

		bool m_enabled { false };
	
		void UpdateShip( dd::TransformComponent& transform, neut::ShipComponent& ship, float delta_t );

		void CreateShip( ddc::EntitySpace& entities );

		virtual void DrawDebugInternal( ddc::EntitySpace& entities ) override;
	};
}
