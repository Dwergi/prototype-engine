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
	struct Renderer;
	struct RenderData;

	struct RenderManager : dd::IDebugPanel
	{
		RenderManager();
		~RenderManager();

		//
		// Initialize all registered renderers.
		//
		void Initialize( ddc::EntitySpace& entities );
		void Shutdown();

		//
		// Render all the registered renderers.
		//
		void Render( ddc::EntitySpace& entities, const ddr::ICamera& camera, float delta_t );

		//
		// Register a renderer.
		//
		void Register( ddr::Renderer& renderer );

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
		
		std::vector<ddr::Renderer*> m_renderers;

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		float m_time { 0 };

		bool m_debugDrawNormals { false };
		bool m_debugDrawDepth { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_reloadShaders { false };
		
		// TODO: This shouldn't be here.
		ddr::MeshHandle m_cube;

		ddr::CommandBuffer m_commands; 
		ddr::UniformStorage m_uniforms;

		void CreateFrameBuffer( glm::ivec2 size );

		void RenderDebug( const ddr::RenderData& data, ddr::Renderer& debug_render );

		void BeginRender( const ddc::EntitySpace& entities, const ddr::ICamera& camera );
		void EndRender( ddr::UniformStorage& uniforms, const ddr::ICamera& camera );

		using CallRendererFn = std::function<void(Renderer&, const RenderData&)>;
		void CallRenderer( ddr::Renderer& renderer, ddc::EntitySpace& entities, const ddr::ICamera& camera, const CallRendererFn& fn );

		virtual const char* GetDebugTitle() const override { return "Renderer"; }
	};
}
