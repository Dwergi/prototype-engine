//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "EntityHandle.h"
#include "IDebugPanel.h"
#include "InputAction.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "Ray.h"
#include "FrameBuffer.h"
#include "ShaderHandle.h"
#include "Texture.h"

struct GLFWwindow;

namespace ddr
{
	class ShaderProgram;
}

namespace dd
{
	class ICamera;
	class Input;
	class InputBindings;
	class MeshComponent;
	class TransformComponent;
	class Window;

	struct MousePosition;

	class MousePicking : public ISystem, public IDebugPanel, public IRenderer
	{
	public:

		static const int DownScalingFactor = 2;

		MousePicking( const Window& window, const Input& input );

		virtual void Update( EntityManager& entity_manager, float dt ) override;

		EntityHandle GetFocusedMesh() const { return m_focusedMesh; }
		EntityHandle GetSelectedMesh() const { return m_selectedMesh; }

		int GetEntityHandleAt( glm::vec2 mouse_pos ) const;
		float GetDepthAt( glm::vec2 mouse_pos ) const;

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit() override;
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera, ddr::UniformStorage& uniforms ) override;

		virtual bool ShouldRenderDebug() const override { return m_renderDebug; }
		virtual void RenderDebug() override;

	private:

		const Input& m_input;
		const Window& m_window;

		bool m_select { false };
		bool m_enabled { false };
		bool m_renderDebug { false };

		EntityHandle m_selectedMesh;
		EntityHandle m_focusedMesh;

		ddr::ShaderHandle m_shader;
		ddr::Texture m_idTexture;
		ddr::Texture m_depthTexture;
		
		ddr::FrameBuffer m_framebuffer;

		Buffer<byte> m_lastIDBuffer;
		Buffer<byte> m_lastDepthBuffer;

		glm::vec2 m_position;
		int m_handle { 0 };
		float m_depth { 0.0f };

		glm::ivec2 m_previousSize;

		virtual void DrawDebugInternal() override;

		void CreateFrameBuffer( glm::ivec2 window_size );

		Ray GetScreenRay( const ICamera& camera, const MousePosition& pos ) const;
		void HitTestMesh( EntityHandle entity, const MeshComponent* mesh_cmp, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );

		void RenderMesh( const dd::ICamera& camera, ddr::ShaderProgram& shader, EntityHandle entity, const MeshComponent* mesh, const TransformComponent* transform );
	};
}
