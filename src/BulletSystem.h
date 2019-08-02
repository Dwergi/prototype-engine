//
// BulletSystem.h - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "Entity.h"
#include "IDebugPanel.h"
#include "InputAction.h"
#include "System.h"

namespace ddr
{
	struct ICamera;
}

namespace dd
{
	struct BulletComponent;
	struct InputBindings;
	struct HitState;
	struct TransformComponent;

	struct BulletSystem : ddc::System, IDebugPanel
	{
		BulletSystem();

		void BindActions( InputBindings& bindings );

		virtual void Initialize( ddc::World& world ) override;
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

		void HandleInput( InputAction action, InputType type );
		void FireBullet( ddc::World& world, const ddr::ICamera& camera );
		void KillBullet( ddc::World& world, ddc::Entity entity, dd::BulletComponent& bullet );
		bool HitTestDynamicMeshes( dd::BulletComponent& bullet, dd::TransformComponent& transform, const ddc::DataBuffer& meshes, float delta_t, glm::vec3& out_pos );

		virtual void DrawDebugInternal( ddc::World& world ) override;
	};
}