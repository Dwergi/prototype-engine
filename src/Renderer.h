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
	class ShaderProgram;
	class Window;
	class MeshComponent;
	class TransformComponent;

	class Renderer
	{
	public:

		Renderer();
		~Renderer();

		void Initialize( Window& window, EntityManager& entity_manager );

		//
		// Render a full frame.
		// Does NOT call Window::Swap, which is done in main loop because of debug UI stuff.
		//
		void Render( float delta_t );

		//
		// Retrieve the active camera.
		//
		Camera& GetCamera() const;

	private:

		Window* m_window;
		Camera* m_camera;
		ShaderProgram* m_defaultShader;
		Vector<ShaderHandle> m_shaders;
		EntityManager* m_entityManager;

		EntityHandle m_xAxis;
		EntityHandle m_yAxis;
		EntityHandle m_zAxis;
		bool m_drawAxes;

		int m_meshCount;

		// TODO: Delete these and use actual camera values.
		Camera* m_debugCamera;
		Frustum* m_debugFrustum;
		bool m_debugFrustumEnabled;
		bool m_debugHighlightMeshes;
		float m_debugFrustumFar;
		float m_debugFrustumNear;
		int m_debugFrustumMeshCount;

		// TODO: Introduce renderer freezing.

		float m_debugFocusedMeshDistance;
		EntityHandle m_debugFocusedMesh;

		void DrawDebugUI();
		EntityHandle CreateMeshEntity( EntityManager& entity_manager, const char* meshName, ShaderProgram& shader, glm::vec4& colour, const glm::mat4& transform );
		void SetRenderState();

		void CreateDebugFrustum( Window& window );
		void RenderDebugFrustum();

		void HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp );
		void RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp );
	};
}
