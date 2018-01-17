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
#include "FrameBuffer.h"
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

		static const int DownScalingFactor = 2;

		MousePicking( const Window& window, const ICamera& camera, const Input& input );

		virtual void Update( EntityManager& entity_manager, float dt ) override;

		EntityHandle GetFocusedMesh() const { return m_focusedMesh; }
		EntityHandle GetSelectedMesh() const { return m_selectedMesh; }

		int GetEntityHandleAt( glm::vec2 mouse_pos ) const;
		float GetDepthAt( glm::vec2 mouse_pos ) const;

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit( const EntityManager& entity_manager, const ICamera& camera ) override;
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera ) override;

		virtual bool ShouldRenderDebug() const override { return m_renderDebug; }
		virtual void RenderDebug() override;

	private:

		const ICamera& m_camera;
		const Input& m_input;
		const Window& m_window;

		bool m_select { false };
		bool m_enabled { false };
		bool m_renderDebug { false };

		EntityHandle m_selectedMesh;
		EntityHandle m_focusedMesh;

		ShaderHandle m_shader;
		Texture m_idTexture;
		Texture m_depthTexture;
		
		FrameBuffer m_framebuffer;

		Buffer<byte> m_lastIDBuffer;
		Buffer<byte> m_lastDepthBuffer;

		glm::vec2 m_position;
		int m_handle { 0 };
		float m_depth { 0.0f };

		glm::ivec2 m_previousSize;

		virtual void DrawDebugInternal() override;

		void CreateFrameBuffer( glm::ivec2 window_size );

		Ray GetScreenRay( const MousePosition& pos ) const;
		void HitTestMesh( EntityHandle entity, const MeshComponent* mesh_cmp, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );

		void RenderMesh( const ICamera& camera, ShaderProgram& shader, EntityHandle entity, const MeshComponent* mesh, const TransformComponent* transform );
	};
}
