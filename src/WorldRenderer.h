//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "FrameBuffer.h"
#include "IDebugPanel.h"
#include "System.h"
#include "MeshHandle.h"
#include "Texture.h"

namespace dd
{
	class MeshComponent;
	class MousePicking;
	class TransformComponent;
	class Window;
}

namespace ddr
{
	class ICamera;
	class Renderer;
	struct Frustum;
	struct RenderData;
	struct UniformStorage;

	class WorldRenderer : public dd::IDebugPanel
	{
	public:

		WorldRenderer( const dd::Window& window );
		~WorldRenderer();

		//
		// Initialize the renderer.
		//
		void InitializeRenderer();

		//
		// Render all the registered renderers.
		//
		void Render( const ddc::World& world, const ddr::ICamera& camera );

		void ShutdownRenderer() {}

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

		const dd::Window& m_window;
		
		FrameBuffer m_framebuffer;
		Texture m_colourTexture;
		Texture m_depthTexture;
		
		ddc::Entity m_xAxis;
		ddc::Entity m_yAxis;
		ddc::Entity m_zAxis;

		std::vector<ddc::Entity> m_debugLights;
		std::vector<ddr::Renderer*> m_renderers;

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		bool m_debugDrawStandard { true };
		bool m_debugDrawDepth { false };
		bool m_debugDrawAxes { true };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_debugMeshGridCreated { false };
		bool m_createDebugMeshGrid { false };
		bool m_reloadShaders { false };
	
		ddc::Entity m_deleteLight;
		bool m_createLight { false };

		MeshHandle m_unitCube;

		ddr::UniformStorage* m_uniforms { nullptr };

		void CreateFrameBuffer( glm::ivec2 size );

		//void CreateDebugMeshGrid( ddc::World& world );
		ddc::Entity CreatePointLight( ddc::World& world );
		void UpdateDebugPointLights( ddc::World& world );

		void SetRenderState();
		void RenderDebug( const ddr::RenderData& data, ddr::Renderer& debug_render );

		void BeginRender( const ddc::World& world, const ddr::ICamera& camera );
		void EndRender( ddr::UniformStorage& uniforms, const ddr::ICamera& camera );

		void CallRenderer( ddr::Renderer& renderer, const ddc::World& world, const ddr::ICamera& camera, std::function<void( Renderer&, const RenderData& )> fn );

		virtual const char* GetDebugTitle() const override { return "Renderer"; }
	};
}
