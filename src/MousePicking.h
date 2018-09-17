//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "Entity.h"
#include "FrameBuffer.h"
#include "IAsyncHitTest.h"
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
	class Mesh;
	struct ShaderProgram;
}

namespace dd
{
	struct IAsyncHitTest;
	struct HitState;

	struct AABB;
	class Input;
	class InputBindings;
	struct MeshComponent;
	struct MousePosition;
	struct RayComponent;
	struct TransformComponent;
	class Window;
	
	class MousePicking : public IDebugPanel, public ddr::Renderer
	{
	public:

		static const int DownScalingFactor = 2;

		MousePicking( const Window& window, const Input& input, IAsyncHitTest& hit_test );

		int GetEntityIDAt( glm::vec2 mouse_pos ) const;
		float GetDepthAt( glm::vec2 mouse_pos ) const;

		void BindActions( InputBindings& bindings );

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit( ddc::World& world ) override;
		virtual void Render( const ddr::RenderData& data ) override;

		virtual bool ShouldRenderDebug() const override { return m_renderDebug; }
		virtual void RenderDebug( const ddr::RenderData& data ) override;

	private:

		const Input& m_input;
		const Window& m_window;
		dd::IAsyncHitTest& m_hitTest;

		bool m_select { false };
		bool m_enabled { true };
		bool m_renderDebug { false };
		bool m_rayTest { true };

		ddc::Entity m_selected;
		ddc::Entity m_focused;

		glm::vec2 m_position;
		float m_depth { 0.0f };

		ddr::ShaderHandle m_shader;
		ddr::Texture m_idTexture;
		ddr::Texture m_depthTexture;
		
		ddr::FrameBuffer m_framebuffer;

		Buffer<byte> m_lastIDBuffer;
		Buffer<byte> m_lastDepthBuffer;

		bool m_visualizeRay { false };
		ddc::Entity m_previousRay;

		dd::HitState m_previousHitState;
		const dd::HitState* m_hitState { nullptr };

		glm::ivec2 m_previousSize;

		virtual void DrawDebugInternal( const ddc::World& world ) override;

		void CreateFrameBuffer( glm::ivec2 window_size );

		Ray GetScreenRay( const ddr::ICamera& camera ) const;

		void HandleInput( InputAction action, InputType type );

		ddc::Entity HitTestRender( const ddr::RenderData& data );
		ddc::Entity HitTestRay( const ddr::RenderData& data );
	};
}
