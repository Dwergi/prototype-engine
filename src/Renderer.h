//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "EntityHandle.h"
#include "FrameBuffer.h"
#include "IDebugPanel.h"
#include "ISystem.h"
#include "MeshHandle.h"
#include "Texture.h"

namespace dd
{
	class EntityManager;
	class ICamera;
	class IRenderer;
	class MeshComponent;
	class MousePicking;
	class TransformComponent;
	class Window;
}

namespace ddr
{
	class Frustum;
	class ShaderProgram;
	class UniformStorage;

	class Renderer : public dd::IDebugPanel, public dd::ISystem
	{
	public:

		Renderer( const dd::Window& window );
		~Renderer();

		void Shutdown();

		virtual void Initialize( dd::EntityManager& entity_manager ) override;

		virtual void Update( dd::EntityManager& entity_manager, float delta_t ) override;

		//
		// Setup render states, uniforms, and the like for other renderers.
		//
		void BeginRender( const dd::EntityManager& entity_manager, const dd::ICamera& camera );

		//
		// Render all the registered renderers.
		//
		void Render( const dd::EntityManager& entity_manager, const dd::ICamera& camera );

		//
		// Complete the render. Does *NOT* call Swap - that's in the main loop.
		//
		void EndRender( const dd::ICamera& camera );

		//
		// Initialize the renderer.
		//
		void RenderInit();

		//
		// Register a renderer.
		//
		void Register( dd::IRenderer& renderer );

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
		
		dd::EntityHandle m_xAxis;
		dd::EntityHandle m_yAxis;
		dd::EntityHandle m_zAxis;

		std::vector<dd::EntityHandle> m_debugLights;
		std::vector<dd::IRenderer*> m_renderers;

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		bool m_debugDrawStandard { true };
		bool m_debugDrawDepth { false };
		bool m_debugDrawAxes { true };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_debugMeshGridCreated { false };
		bool m_createDebugMeshGrid { false };
		bool m_reloadShaders { false };
	
		dd::EntityHandle m_deleteLight;
		bool m_createLight { false };

		MeshHandle m_unitCube;

		ddr::UniformStorage* m_uniforms { nullptr };

		void CreateFrameBuffer( glm::ivec2 size );

		void CreateDebugMeshGrid( dd::EntityManager& entityManager );
		dd::EntityHandle CreateMeshEntity( dd::EntityManager& entityManager, MeshHandle mesh_h, glm::vec4 colour, const glm::mat4& transform );
		dd::EntityHandle CreatePointLight( dd::EntityManager& entityManager );
		void UpdateDebugPointLights( dd::EntityManager& entityManager );

		void SetRenderState();
		void RenderDebug( dd::IRenderer& debug_render );

		virtual const char* GetDebugTitle() const override { return "Renderer"; }
	};
}
