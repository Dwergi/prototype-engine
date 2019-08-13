//
// MousePicking.h - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#pragma once

#include "FrameBuffer.h"
#include "IAsyncHitTest.h"
#include "IDebugPanel.h"
#include "InputAction.h"
#include "Material.h"
#include "Ray.h"
#include "IRenderer.h"
#include "System.h"
#include "Texture.h"

struct GLFWwindow;

namespace ddr
{
	struct ICamera;
}

namespace ddm
{
	struct AABB;
}

namespace dd
{
	struct IAsyncHitTest;
	struct HitState;
	struct IInputSource;
	struct InputKeyBindings;
	struct MeshComponent;
	struct MousePosition;
	struct RayComponent;
	struct TransformComponent;
	struct Window;
	
	struct MousePicking : IDebugPanel, ddr::IRenderer
	{
		static const int DownScalingFactor = 2;

		MousePicking();

		int GetEntityIDAt( glm::vec2 mouse_pos ) const;
		float GetDepthAt( glm::vec2 mouse_pos ) const;

		virtual const char* GetDebugTitle() const override { return "Mouse Picking"; }
		
		virtual void RenderInit( ddc::EntitySpace& entities ) override;
		virtual void RenderUpdate( ddc::EntitySpace& entities ) override;
		virtual void Render( const ddr::RenderData& render_data ) override;

		virtual bool ShouldRenderDebug() const override { return m_renderDebug; }
		virtual void RenderDebug( const ddr::RenderData& data ) override;

	private:

		bool m_enabled { false };
		bool m_renderDebug { false };
		bool m_rayTest { true };

		ddc::Entity m_selected;
		ddc::Entity m_focused;

		glm::vec2 m_position;
		float m_depth { 0.0f };

		ddr::MaterialHandle m_material;
		ddr::Texture m_idTexture;
		ddr::Texture m_depthTexture;
		
		ddr::FrameBuffer m_framebuffer;

		Buffer<byte> m_lastIDBuffer;
		Buffer<byte> m_lastDepthBuffer;

		bool m_visualizeRay { false };
		ddc::Entity m_previousRay;

		ddc::Entity m_hitEntity;

		dd::HitResult m_hitResult;
		dd::HitHandle m_pendingHit;

		glm::ivec2 m_previousSize;

		virtual void DrawDebugInternal() override;

		void CreateFrameBuffer( glm::ivec2 window_size );

		ddm::Ray GetScreenRay( const ddr::ICamera& camera, float length ) const;

		ddc::Entity HitTestRender( const ddr::RenderData& data );
		ddc::Entity HitTestRay( const ddr::RenderData& data, bool select);
	};
}
