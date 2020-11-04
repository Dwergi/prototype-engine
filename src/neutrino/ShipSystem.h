//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "IDebugPanel.h"
#include "InputAction.h"

#include "ddc/System.h"

#include "ddr/Mesh.h"

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

	private:

		glm::vec3 m_nextCameraPos;

		ddc::Entity m_lastShip;
		ddr::MeshHandle m_shipMesh;

		bool m_enabled { false };
	
		void UpdateShip( dd::TransformComponent& transform, neut::ShipComponent& ship, float delta_t );

		void CreateShip( ddc::EntityLayer& entities );

		// IDebugPanel implementation
		virtual void DrawDebugInternal() override; 
		virtual const char* GetDebugTitle() const override { return "Ship"; }

		// System implementation
		virtual void Initialize(ddc::EntityLayer& entities) override {}
		virtual void Update(ddc::UpdateData& data) override;
		virtual void Shutdown(ddc::EntityLayer& entities) override;
	};
}
