//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "EntityHandle.h"
#include "InputAction.h"
#include "Ray.h"

struct GLFWwindow;

namespace dd
{
	class Camera;
	class Input; 
	class InputBindings;
	class MeshComponent;
	class Window;

	struct MousePosition;

	class MousePicking
	{
	public:

		MousePicking( Window& window, Camera& camera, Input& input );

		void UpdatePicking( const EntityManager& entity_manager );

		EntityHandle GetFocusedMesh() const { return m_focusedMesh; }
		EntityHandle GetSelectedMesh() const { return m_selectedMesh; }

		void BindActions( InputBindings& bindings );

	private:

		Camera* m_camera;
		Input* m_input;
		Window* m_window;

		bool m_select;
		bool m_enabled;

		EntityHandle m_selectedMesh;
		EntityHandle m_focusedMesh;

		Ray GetScreenRay( const MousePosition& pos ) const;
		void HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );
	};
}
