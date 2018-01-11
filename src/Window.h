//
// Window.h - Wrapper around GLFW windowing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

struct GLFWwindow;

namespace dd
{
	class Window
	{
	public:

		Window( glm::ivec2 resolution, const char* title );
		~Window();

		void Close();

		void Resize( int resX, int resY );

		bool IsFocused() const;
		bool IsValid() const;
		bool IsContextValid() const;

		bool ShouldClose() const;
		void SetToClose() const;

		void MakeBorderless();

		void Swap();

		glm::ivec2 GetSize() const { return m_size; }
		int GetWidth() const { return m_size.x; }
		int GetHeight() const { return m_size.y; }

		GLFWwindow* GetInternalWindow() const { return m_glfwWindow; }

		static void OnFramebufferResize( GLFWwindow* window, int width, int height );
		static void OnWindowResize( GLFWwindow* window, int width, int height );

	private:
		static Window* m_pInstance;

		GLFWwindow* m_glfwWindow;
		String32  m_title;

		glm::ivec2 m_size;
		bool m_focused;
	};
}