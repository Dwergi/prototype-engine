//
// RenderManager.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "CommandBuffer.h"
#include "FrameBuffer.h"
#include "IDebugPanel.h"
#include "Mesh.h"
#include "RenderState.h"
#include "System.h"
#include "Texture.h"
#include "Uniforms.h"

namespace dd
{
	struct IWindow;
}

namespace ddr
{
	struct ICamera;
	struct IRenderer;
	struct RenderData;

	struct RenderManager : dd::IDebugPanel
	{
		RenderManager();
		~RenderManager();

		//
		// Initialize all registered renderers.
		//
		void Initialize();

		//
		// Shutdown all registered renderers.
		//
		void Shutdown();

		//
		// Render all the registered renderers.
		//
		void Render( ddc::EntityLayer& entities, const ddr::ICamera& camera, float delta_t );

		//
		// Register a renderer.
		//
		void Register( ddr::IRenderer& renderer );

	protected:

		//
		// Draw the debug menu.
		//
		virtual void DrawDebugInternal() override;

	private:
		
		FrameBuffer m_framebuffer;
		Texture m_colourTexture;
		Texture m_depthTexture;

		RenderState m_defaultState;
		RenderState m_depthState;
		
		std::vector<ddr::IRenderer*> m_renderers;

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		float m_time { 0 };

		bool m_debugDrawNormals { false };
		bool m_debugDrawDepth { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_reloadShaders { false };
		
		ddr::UniformStorage m_uniforms;

		void CreateFrameBuffer( glm::ivec2 size );

		void RenderDebug( const ddr::RenderData& data, ddr::IRenderer& debug_render );

		void BeginRender( const ddc::EntityLayer& entities, const ddr::ICamera& camera );
		void EndRender( ddr::UniformStorage& uniforms, const ddr::ICamera& camera );

		void FillRenderData(ddr::IRenderer& renderer, ddc::EntityLayer& entities, const ddr::ICamera& camera, float delta_t);

		virtual const char* GetDebugTitle() const override { return "Renderer"; }
	};
}
