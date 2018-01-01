//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "EntityHandle.h"
#include "IDebugDraw.h"
#include "InputAction.h"
#include "ISystem.h"

namespace dd
{
	class FPSCamera;
	class InputBindings;
	class ShipComponent;
	class TransformComponent;

	class ShipSystem : public ISystem, public IDebugDraw
	{
	public:

		ShipSystem( FPSCamera& camera );
		ShipSystem( const ShipSystem& other ) = delete;

		void BindActions( InputBindings& bindings );

		void CreateShip( EntityManager& entity_manager );

		virtual void Update( EntityManager& entity_manager, float dt ) override;
		virtual void PostRender( EntityManager& entity_manager, float dt ) override;

		void Enable( bool enabled ) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		virtual const char* GetDebugTitle() const override { return "Ship"; }

	protected:

		virtual void DrawDebugInternal() override;

	private:

		FPSCamera& m_camera;
		glm::vec3 m_nextCameraPos;

		EntityHandle m_lastShip;

		DenseMap<InputAction, bool> m_inputs;
		bool m_enabled { false };
	
		void HandleInput( InputAction action, InputType type );
		void UpdateShip( EntityHandle entity, ComponentHandle<TransformComponent> transform, ComponentHandle<ShipComponent> ship, float delta_t );
	};
}
