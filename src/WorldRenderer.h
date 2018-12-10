//
// WorldRenderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "FrameBuffer.h"
#include "IDebugPanel.h"
#include "Mesh.h"
#include "RenderState.h"
#include "System.h"
#include "Texture.h"

namespace dd
{
	class Window;
}

namespace ddr
{
	struct ICamera;
	class Renderer;
	struct RenderData;
	struct UniformStorage;

	class WorldRenderer : public dd::IDebugPanel
	{
	public:

		WorldRenderer( const dd::Window& window );
		~WorldRenderer();

		//
		// Initialize all registered renderers.
		//
		void InitializeRenderers( ddc::World& world );

		//
		// Render all the registered renderers.
		//
		void Render( ddc::World& world, const ddr::ICamera& camera, float delta_t );

		void ShutdownRenderer() {}

		//
		// Register a renderer.
		//
		void Register( ddr::Renderer& renderer );

	protected:

		//
		// Draw the debug menu.
		//
		virtual void DrawDebugInternal( ddc::World& world ) override;

	private:

		const dd::Window& m_window;
		
		FrameBuffer m_framebuffer;
		Texture m_colourTexture;
		Texture m_depthTexture;

		RenderState m_defaultState;
		RenderState m_depthState;
		
		ddc::Entity m_xAxis;
		ddc::Entity m_yAxis;
		ddc::Entity m_zAxis;

		std::vector<ddr::Renderer*> m_renderers;

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		float m_time { 0 };

		bool m_debugDrawNormals { false };
		bool m_debugDrawDepth { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_reloadShaders { false };
		
		MeshHandle m_cube;

		ddr::UniformStorage* m_uniforms { nullptr };

		void CreateFrameBuffer( glm::ivec2 size );

		void RenderDebug( const ddr::RenderData& data, ddr::Renderer& debug_render );

		void BeginRender( const ddc::World& world, const ddr::ICamera& camera );
		void EndRender( ddr::UniformStorage& uniforms, const ddr::ICamera& camera );

		void CallRenderer( ddr::Renderer& renderer, ddc::World& world, const ddr::ICamera& camera, std::function<void( Renderer&, const RenderData& )> fn ) const;

		virtual const char* GetDebugTitle() const override { return "Renderer"; }
	};
}
