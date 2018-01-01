//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "EntityHandle.h"
#include "IDebugDraw.h"
#include "InputAction.h"
#include "ISystem.h"
#include "Ray.h"

struct GLFWwindow;

namespace dd
{
	class ICamera;
	class Input; 
	class InputBindings;
	class MeshComponent;
	class Window;

	struct MousePosition;

	class MousePicking : public ISystem, public IDebugDraw
	{
	public:

		MousePicking( const Window& window, const ICamera& camera, const Input& input );

		virtual void Update( EntityManager& entity_manager, float dt ) override;

		EntityHandle GetFocusedMesh() const { return m_focusedMesh; }
		EntityHandle GetSelectedMesh() const { return m_selectedMesh; }

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }

	protected:

		virtual void DrawDebugInternal() override;

	private:

		const ICamera& m_camera;
		const Input& m_input;
		const Window& m_window;

		bool m_select { false };
		bool m_enabled { false };

		EntityHandle m_selectedMesh;
		EntityHandle m_focusedMesh;

		Ray GetScreenRay( const MousePosition& pos ) const;
		void HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );
	};
}
