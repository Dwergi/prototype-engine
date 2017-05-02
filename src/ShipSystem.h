//
// ShipSystem.h - System that updates ships.
// Copyright (C) Sebastian Nordgren 
// April 28th 2017
//

#pragma once

#include "EntityHandle.h"
#include "InputAction.h"
#include "ISystem.h"

namespace dd
{
	class Camera;
	class InputBindings;
	class ShipComponent;
	class TransformComponent;

	class ShipSystem : public ISystem
	{
	public:

		ShipSystem( Camera& camera );
		ShipSystem( const ShipSystem& other ) = delete;

		void BindActions( InputBindings& bindings );

		void CreateShip( EntityManager& entity_manager );

		virtual void Update( EntityManager& entity_manager, float dt ) override;

		void Enable( bool enabled ) { m_enabled = enabled; }

	private:

		Camera& m_camera; 
		DenseMap<InputAction, bool> m_inputs;
		bool m_enabled;
	
		void HandleInput( InputAction action, InputType type );
		void UpdateShip( EntityHandle entity, ComponentHandle<TransformComponent> transform, ComponentHandle<ShipComponent> ship, float delta_t );
	};
}
