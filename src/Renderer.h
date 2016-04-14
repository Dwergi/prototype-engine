//
// Renderer.h - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

namespace dd
{
	class Camera;
	class ShaderProgram;
	class Mesh;
	class Window;

	class Renderer
	{
	public:

		Renderer();
		~Renderer();

		void Init( Window& window );

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
		Vector<Mesh*> m_meshes;
		Vector<ShaderProgram*> m_shaders;
	};
}
