//
// BulletSystem.h - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "InputAction.h"
#include "System.h"

namespace ddr
{
	struct ICamera;
}

namespace dd
{
	struct InputKeyBindings;
	struct HitState;
	struct TransformComponent;
}

namespace neut
{
	struct BulletComponent;

	struct BulletSystem : ddc::System, dd::IDebugPanel
	{
		BulletSystem();

		virtual void Initialize( ddc::EntityLayer& entities ) override;
		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const char* GetDebugTitle() const override { return "Bullets"; }

	private:

		bool m_fireBullet { false };
		int m_count { 0 };

		float m_speed { 0 };
		glm::vec3 m_colour;
		float m_scale { 0 };
		float m_intensity { 0 };
		float m_attenuation { 0 };

		void FireBullet( ddc::EntityLayer& entities, const ddr::ICamera& camera );
		void KillBullet( ddc::EntityLayer& entities, ddc::Entity entity, neut::BulletComponent& bullet );
		bool HitTestDynamicMeshes(neut::BulletComponent& bullet, dd::TransformComponent& transform, const ddc::UpdateDataBuffer& meshes, float delta_t, glm::vec3& out_pos );

		virtual void DrawDebugInternal() override;
	};
}