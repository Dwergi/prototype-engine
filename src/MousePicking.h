//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "Entity.h"
#include "FrameBuffer.h"
#include "IDebugPanel.h"
#include "InputAction.h"
#include "Renderer.h"
#include "System.h"
#include "Ray.h"
#include "ShaderHandle.h"
#include "Texture.h"

struct GLFWwindow;

namespace ddr
{
	class ICamera;
	struct ShaderProgram;
}

namespace dd
{
	class Input;
	class InputBindings;
	class MeshComponent;
	class TransformComponent;
	class Window;

	struct MousePosition;

	class MousePicking : public ddc::System, public IDebugPanel, public ddr::Renderer
	{
	public:

		static const int DownScalingFactor = 2;

		MousePicking( const Window& window, const Input& input );

		virtual void Initialize( ddc::World& ) override {}
		virtual void Update( const ddc::UpdateData& data, float delta_t ) override;
		virtual void Shutdown( ddc::World& ) override {}

		ddc::Entity GetFocusedMesh() const { return m_focusedMesh; }
		ddc::Entity GetSelectedMesh() const { return m_selectedMesh; }

		int GetEntityHandleAt( glm::vec2 mouse_pos ) const;
		float GetDepthAt( glm::vec2 mouse_pos ) const;

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit() override;
		virtual void Render( const ddr::RenderData& data ) override;

		virtual bool ShouldRenderDebug() const override { return m_renderDebug; }
		virtual void RenderDebug( const ddr::RenderData& data ) override;

	private:

		const Input& m_input;
		const Window& m_window;

		bool m_select { false };
		bool m_enabled { false };
		bool m_renderDebug { false };

		ddc::Entity m_selectedMesh;
		ddc::Entity m_focusedMesh;

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

		virtual void DrawDebugInternal( const ddc::World& world ) override;

		void CreateFrameBuffer( glm::ivec2 window_size );

		Ray GetScreenRay( const ddr::ICamera& camera, const MousePosition& pos ) const;
		void HitTestMesh( ddc::Entity entity, const MeshComponent& mesh_cmp, const Ray& mouse_ray, float& nearest_distance );

		void HandleInput( InputAction action, InputType type );

		void RenderMesh( ddr::UniformStorage& uniforms, ddr::ShaderProgram& shader, ddc::Entity entity, const MeshComponent& mesh_cmp, const TransformComponent& transform_cmp );
	};
}
