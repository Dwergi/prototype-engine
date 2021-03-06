//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "BVHTree.h"
#include "IDebugPanel.h"

#include "ddc/System.h"

namespace dd
{
	struct PhysicsSystem : ddc::System, dd::IDebugPanel
	{
		PhysicsSystem();

		virtual void Initialize( ddc::EntityLayer& entities ) override;
		virtual void Update( ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Physics"; }

	private:
		virtual void DrawDebugInternal() override;
		
		glm::vec3 m_gravity { 0, -9.81, 0 };

		int m_collisions { 0 };
		BVHTree m_broadphase;
	};
}