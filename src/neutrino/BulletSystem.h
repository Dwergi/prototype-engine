//
// BulletSystem.h - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "IDebugPanel.h"
#include "InputAction.h"

#include "ddc/System.h"

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

		virtual void Initialize(ddc::EntityLayer& layer) override;
		virtual void Update(ddc::UpdateData& update) override;

		virtual const char* GetDebugTitle() const override { return "Bullets"; }

	private:

		bool m_fireBullet { false };
		int m_count { 0 };

		float m_speed { 0 };
		glm::vec3 m_colour;
		float m_scale { 0 };
		float m_intensity { 0 };
		float m_attenuation { 0 };

		ddc::MessageType m_bulletHitMessage;

		void FireBullet(ddc::UpdateData& update, const ddr::ICamera& camera);
		void KillBullet(ddc::UpdateData& update, ddc::Entity entity, neut::BulletComponent& bullet);
		bool HitTestDynamicMeshes(neut::BulletComponent& bullet, dd::TransformComponent& transform, const ddc::UpdateBufferView& meshes, float delta_t, glm::vec3& out_pos);

		virtual void DrawDebugInternal() override;
	};
}