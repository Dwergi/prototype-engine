//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "EntityHandle.h"
#include "IDebugDraw.h"
#include "InputAction.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "Ray.h"
#include "RenderToTexture.h"
#include "ShaderHandle.h"
#include "Texture.h"

struct GLFWwindow;

namespace dd
{
	class ICamera;
	class Input; 
	class InputBindings;
	class MeshComponent;
	class ShaderProgram;
	class TransformComponent;
	class Window;

	struct MousePosition;

	class MousePicking : public ISystem, public IDebugDraw, public IRenderer
	{
	public:

		MousePicking( const Window& window, const ICamera& camera, const Input& input );

		virtual void Update( EntityManager& entity_manager, float dt ) override;

		EntityHandle GetFocusedMesh() const { return m_focusedMesh; }
		EntityHandle GetSelectedMesh() const { return m_selectedMesh; }

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit( const EntityManager& entity_manager, const ICamera& camera ) override;
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera ) override;

		RenderToTexture& GetRTT() { return m_rtt; }

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

		ShaderHandle m_shader;
		Texture m_texture;
		RenderToTexture m_rtt;
		Buffer<byte> m_textureData;

		glm::ivec2 m_position;
		int m_handle { 0 };

		Ray GetScreenRay( const MousePosition& pos ) const;
		void HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );

		void RenderMesh( const ICamera& camera, ShaderProgram& shader, EntityHandle entity, const MeshComponent* mesh, const TransformComponent* transform );
	};
}
