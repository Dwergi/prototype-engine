//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "EntityHandle.h"
#include "FrameBuffer.h"
#include "IDebugDraw.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

namespace dd
{
	class ICamera;
	class EntityManager;
	class Frustum;
	class MeshComponent;
	class MousePicking;
	class ShaderProgram;
	class TransformComponent;
	class Window;

	struct Fog
	{
		bool Enabled { true };
		float Distance { 1000.0f };
		glm::vec3 Colour { 0.6, 0.7, 0.8 };

		static Fog* Get();
	};

	class Renderer : public IDebugDraw, public ISystem
	{
	public:

		Renderer( const Window& window );
		~Renderer();

		void Shutdown();

		virtual void Initialize( EntityManager& entity_manager ) override;

		virtual void Update( EntityManager& entity_manager, float delta_t ) override;

		void BeginRender( const ICamera& camera );
		void EndRender( const ICamera& camera );

		//
		// Render a full frame.
		// Does NOT call Window::Swap, which is done in main loop because of debug UI stuff.
		//
		void Render( const EntityManager& entityManager, const ICamera& camera );

		//
		// Initialize the renderer.
		//
		void RenderInit( const EntityManager& entityManager, const ICamera& camera );

		//
		// Allow the renderer to render debug.
		//
		void RenderDebug( IRenderer& debug_render );

		//
		// Set the mouse picking helper to use.
		//
		void SetMousePicking( MousePicking* mouse_picking ) { m_mousePicking = mouse_picking; }

		virtual const char* GetDebugTitle() const override { return "Renderer"; }

	protected:

		//
		// Draw the debug menu.
		//
		virtual void DrawDebugInternal() override;

	private:

		const Window& m_window;
		Frustum* m_frustum;
		MousePicking* m_mousePicking;
		
		FrameBuffer m_framebuffer;
		Texture m_colourTexture;
		Texture m_depthTexture;
		
		Vector<ShaderHandle> m_shaders;

		EntityHandle m_xAxis;
		EntityHandle m_yAxis;
		EntityHandle m_zAxis;

		Vector<EntityHandle> m_debugLights;

		int m_meshCount;
		int m_frustumMeshCount;

		glm::ivec2 m_previousSize;

		glm::vec3 m_skyColour { 0.6, 0.7, 0.8 };

		bool m_frustumCulling { true };
		bool m_debugDrawStandard { true };
		bool m_debugDrawAxes { true };
		bool m_debugDrawBounds { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_debugMeshGridCreated { false };
		bool m_createDebugMeshGrid { false };
		bool m_debugWireframe { false };
		bool m_debugFreezeFrustum { false };
		bool m_forceUpdateFrustum { false };
		bool m_reloadShaders { false };
		float m_debugWireframeMaxDistance { 250.0f };

		bool m_debugDrawDepth { false };

		glm::vec3 m_debugWireframeColour;
		float m_debugWireframeWidth { 2.0f };

		glm::vec3 m_debugWireframeEdgeColour;
		float m_debugWireframeEdgeWidth { 0.5f };

		EntityHandle m_deleteLight;
		bool m_createLight { false };

		MeshHandle m_unitCube;

		void CreateFrameBuffer( glm::ivec2 size );

		void CreateDebugMeshGrid( EntityManager& entityManager );
		EntityHandle CreateMeshEntity( EntityManager& entityManager, MeshHandle mesh_h, ShaderHandle shader, glm::vec4 colour, const glm::mat4& transform );
		EntityHandle CreatePointLight( EntityManager& entityManager );
		void UpdateDebugPointLights( EntityManager& entityManager );

		void SetRenderState();

		void RenderMesh( EntityHandle entity, const MeshComponent* mesh_cmp, const TransformComponent* transform_cmp, const Vector<EntityHandle>& pointLights, const ICamera& camera, const MousePicking* mouse_picking );
	};
}
