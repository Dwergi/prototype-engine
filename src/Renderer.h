//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "EntityHandle.h"
#include "IDebugDraw.h"
#include "Mesh.h"
#include "ShaderProgram.h"

namespace dd
{
	class ICamera;
	class DirectionalLightComponent;
	class EntityManager;
	class Frustum;
	class MeshComponent;
	class MousePicking;
	class PointLightComponent;
	class Ray;
	class ShaderProgram;
	class TransformComponent;
	class Window;

	class Renderer : public IDebugDraw
	{
	public:

		Renderer( const Window& window );
		~Renderer();

		void Initialize( const ICamera& camera, EntityManager& entityManager );
		void Shutdown();

		//
		// Render a full frame.
		// Does NOT call Window::Swap, which is done in main loop because of debug UI stuff.
		//
		void Render( EntityManager& entityManager, const ICamera& camera, float delta_t );

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

		Vector<ShaderHandle> m_shaders;

		EntityHandle m_xAxis;
		EntityHandle m_yAxis;
		EntityHandle m_zAxis;

		Vector<EntityHandle> m_debugLights;

		int m_meshCount;
		int m_frustumMeshCount;

		bool m_frustumCulling { true };
		bool m_debugDrawAxes { true };
		bool m_debugDrawBounds { false };
		bool m_debugHighlightFrustumMeshes { false };
		bool m_debugMeshGridCreated { false };
		bool m_createDebugMeshGrid { false };
		bool m_debugWireframe { false };
		bool m_debugFreezeFrustum { false };
		bool m_forceUpdateFrustum { false };

		glm::vec3 m_debugWireframeColour;
		float m_debugWireframeWidth { 2.0f };

		EntityHandle m_deleteLight;
		bool m_createLight { false };

		MeshHandle m_unitCube;

		void CreateDebugMeshGrid( EntityManager& entityManager );
		EntityHandle CreateMeshEntity( EntityManager& entityManager, MeshHandle mesh_h, ShaderHandle shader, glm::vec4 colour, const glm::mat4& transform );
		EntityHandle CreatePointLight( EntityManager& entityManager );
		void UpdateDebugPointLights( EntityManager& entityManager );

		void SetRenderState();

		void RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp, const Vector<EntityHandle>& pointLights, const ICamera& camera, const MousePicking* mouse_picking );
	};
}
