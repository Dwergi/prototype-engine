//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "EntityHandle.h"
#include "FrameBuffer.h"
#include "IDebugDraw.h"
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

	struct Fog
	{
		bool Enabled { true };
		float Distance { 1000.0f };
		glm::vec3 Colour { 0.6, 0.7, 0.8 };
	};

	struct Wireframe
	{
		bool Enabled { false };

		glm::vec3 Colour { 0, 1.0f, 0 };
		float Width { 2.0f };

		glm::vec3 EdgeColour { 0, 0, 0 };
		float EdgeWidth { 0.5f };

		float MaxDistance { 250.0f };
	};

	class Renderer : public dd::IDebugDraw, public dd::ISystem
	{
	public:

		Renderer( const dd::Window& window );
		~Renderer();

		void Shutdown();

		virtual void Initialize( dd::EntityManager& entity_manager ) override;

		virtual void Update( dd::EntityManager& entity_manager, float delta_t ) override;

		void BeginRender( const dd::ICamera& camera );
		void EndRender( const dd::ICamera& camera );

		//
		// Render a full frame.
		// Does NOT call Window::Swap, which is done in main loop because of debug UI stuff.
		//
		void Render( const dd::EntityManager& entityManager, const dd::ICamera& camera );

		//
		// Initialize the renderer.
		//
		void RenderInit( const dd::EntityManager& entityManager, const dd::ICamera& camera );

		//
		// Allow the renderer to render debug.
		//
		void RenderDebug( dd::IRenderer& debug_render );

		//
		// Set the mouse picking helper to use.
		//
		void SetMousePicking( dd::MousePicking* mouse_picking ) { m_mousePicking = mouse_picking; }

		virtual const char* GetDebugTitle() const override { return "Renderer"; }

	protected:

		//
		// Draw the debug menu.
		//
		virtual void DrawDebugInternal() override;

	private:

		const dd::Window& m_window;
		Frustum* m_frustum { nullptr };
		dd::MousePicking* m_mousePicking { nullptr };
		
		FrameBuffer m_framebuffer;
		Texture m_colourTexture;
		Texture m_depthTexture;
		
		dd::EntityHandle m_xAxis;
		dd::EntityHandle m_yAxis;
		dd::EntityHandle m_zAxis;

		std::vector<dd::EntityHandle> m_debugLights;

		int m_meshCount { 0 };
		int m_frustumMeshCount { 0 };

		glm::ivec2 m_previousSize { -1, -1 };

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		bool m_frustumCulling { true };
		bool m_debugDrawStandard { true };
		bool m_debugDrawAxes { true };
		bool m_debugDrawBounds { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_debugMeshGridCreated { false };
		bool m_createDebugMeshGrid { false };
		bool m_debugFreezeFrustum { false };
		bool m_forceUpdateFrustum { false };
		bool m_reloadShaders { false };

		bool m_debugDrawDepth { false };
	
		dd::EntityHandle m_deleteLight;
		bool m_createLight { false };

		Fog m_fog;
		Wireframe m_wireframe;

		MeshHandle m_unitCube;

		void CreateFrameBuffer( glm::ivec2 size );

		void CreateDebugMeshGrid( dd::EntityManager& entityManager );
		dd::EntityHandle CreateMeshEntity( dd::EntityManager& entityManager, MeshHandle mesh_h, glm::vec4 colour, const glm::mat4& transform );
		dd::EntityHandle CreatePointLight( dd::EntityManager& entityManager );
		void UpdateDebugPointLights( dd::EntityManager& entityManager );

		void SetRenderState();

		void RenderMesh( dd::EntityHandle entity, const dd::MeshComponent* mesh_cmp, const dd::TransformComponent* transform_cmp, const std::vector<dd::EntityHandle>& lights,
			const dd::ICamera& camera, const dd::MousePicking* mouse_picking );
	};
}
