//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "System.h"

namespace dd
{
	struct PhysicsSystem : ddc::System, IDebugPanel
	{
		PhysicsSystem();

		virtual void Initialize( ddc::World& world ) override;
		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Physics"; }

	private:
		virtual void DrawDebugInternal( const ddc::World& world ) override;
		
		glm::vec3 m_gravity;
	};
}