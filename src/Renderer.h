//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"

namespace dd
{
	class Camera;
	class EntityManager;
	class ShaderProgram;
	class TerrainSystem;
	class Window;

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

		void DrawDebugUI() const;

		//
		// Retrieve the active camera.
		//
		Camera& GetCamera() const;

		void SetTerrainSystem( TerrainSystem& terrain ) { m_terrain = &terrain; }

	private:

		Window* m_window;
		Camera* m_camera;
		ShaderProgram* m_defaultShader;
		TerrainSystem* m_terrain;
		Vector<ShaderHandle> m_shaders;
		EntityManager* m_entityManager;

		int m_meshCount;

		void CreateMeshEntity( EntityManager& entity_manager, const char* meshName, ShaderProgram& shader, glm::vec4& colour, const glm::mat4& transform );
	};
}
