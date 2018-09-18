//
// BulletSystem.h - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "Entity.h"
#include "InputAction.h"
#include "System.h"

namespace dd
{
	struct IAsyncHitTest;
	class InputBindings;
	struct HitState;

	struct BulletSystem : ddc::System
	{
		BulletSystem( IAsyncHitTest& hit_test );

		void BindActions( InputBindings& bindings );

		virtual void Initialize( ddc::World& world ) override;
		virtual void Update( const ddc::UpdateData& data ) override;

	private:

		bool m_fireBullet { false };

		IAsyncHitTest& m_hitTest;

		void HandleInput( InputAction action, InputType type );
	};
}