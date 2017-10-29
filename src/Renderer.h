//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "DirectionalLight.h"
#include "EntityHandle.h"
#include "IDebugDraw.h"
#include "Mesh.h"
#include "PointLight.h"
#include "ShaderProgram.h"

namespace dd
{
	class Camera;
	class DirectionalLight;
	class EntityManager;
	class Frustum;
	class MeshComponent;
	class MousePicking;
	class PointLight;
	class Ray;
	class ShaderProgram;
	class TransformComponent;
	class Window;

	class Renderer : public IDebugDraw
	{
	public:

		Renderer();
		~Renderer();

		void Initialize( Window& window, EntityManager& entity_manager );
		void Shutdown();

		//
		// Render a full frame.
		// Does NOT call Window::Swap, which is done in main loop because of debug UI stuff.
		//
		void Render( EntityManager& entity_manager, float delta_t );

		//
		// Retrieve the active camera.
		//
		Camera& GetCamera() const;

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

		Window* m_window;
		Camera* m_camera;
		Frustum* m_frustum;
		MousePicking* m_mousePicking;

		Vector<ShaderHandle> m_shaders;

		EntityHandle m_xAxis;
		EntityHandle m_yAxis;
		EntityHandle m_zAxis;
		bool m_drawAxes;

		int m_meshCount;
		int m_frustumMeshCount;

		bool m_debugHighlightFrustumMeshes;
		bool m_debugMeshGridCreated;
		bool m_createDebugMeshGrid;

		DirectionalLight m_directionalLight;
		Vector<PointLight> m_pointLights;

		float m_ambientStrength;
		float m_specularStrength;

		MeshHandle m_unitCube;

		void CreateDebugMeshGrid( EntityManager& entity_manager );
		EntityHandle CreateMeshEntity( EntityManager& entity_manager, MeshHandle mesh_h, ShaderHandle shader, glm::vec4& colour, const glm::mat4& transform );

		void SetRenderState();

		void RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp, const MousePicking* mouse_picking );
	};
}
