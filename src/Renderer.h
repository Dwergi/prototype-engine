//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "EntityHandle.h"
#include "Mesh.h"
#include "ShaderProgram.h"

namespace dd
{
	class Camera;
	class EntityManager;
	class Frustum;
	class MeshComponent;
	class PointLight;
	class ShaderProgram;
	class TransformComponent;
	class Window;

	class Renderer
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
		void Render( float delta_t );

		//
		// Retrieve the active camera.
		//
		Camera& GetCamera() const;

		//
		// Retrieve the point light.
		//
		PointLight& GetLight() const;

	private:

		Window* m_window;
		Camera* m_camera;
		Frustum* m_frustum;

		Vector<ShaderHandle> m_shaders;
		EntityManager* m_entityManager;

		EntityHandle m_xAxis;
		EntityHandle m_yAxis;
		EntityHandle m_zAxis;
		bool m_drawAxes;

		int m_meshCount;
		int m_frustumMeshCount;

		bool m_debugHighlightMeshes;

		bool m_debugHitTestMeshes;
		float m_debugFocusedMeshDistance;
		EntityHandle m_debugFocusedMesh;

		bool m_debugMeshGridCreated;

		PointLight* m_pointLight;
		EntityHandle m_pointLightMesh;

		float m_ambientStrength;
		float m_specularStrength;

		MeshHandle m_unitCube;

		void DrawDebugUI();
		void UpdateDebugLight();

		EntityHandle CreateMeshEntity( EntityManager& entity_manager, MeshHandle mesh_h, ShaderHandle shader, glm::vec4& colour, const glm::mat4& transform );

		void SetRenderState();

		void HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp );
		void RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp );
	};
}
