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
	struct IAsyncHitTest;
	struct InputBindings;
	struct HitState;

	struct BulletSystem : ddc::System, IDebugPanel
	{
		BulletSystem( ddr::ICamera& camera, IAsyncHitTest& hit_test );

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

		ddr::ICamera& m_camera;
		IAsyncHitTest& m_hitTest;

		void HandleInput( InputAction action, InputType type );
		void FireBullet( ddc::World& world );

		virtual void DrawDebugInternal( const ddc::World& world ) override;
	};
}