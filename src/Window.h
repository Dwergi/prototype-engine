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

		Window( int resX, int resY, const char* title );
		~Window();

		void Close();

		void Resize( int resX, int resY );

		bool IsFocused() const;
		bool IsValid() const;
		bool ShouldClose() const;

		void MakeBorderless();

		void Swap();

		int GetWidth() const { return m_sizeX; }
		int GetHeight() const { return m_sizeY; }

		GLFWwindow* GetInternalWindow() const { return m_glfwWindow; }

		static void OnFramebufferResize( GLFWwindow* window, int width, int height );
		static void OnWindowResize( GLFWwindow* window, int width, int height );

	private:
		static Window* m_pInstance;

		GLFWwindow* m_glfwWindow;
		String32  m_title;
		int m_sizeX, m_sizeY;
		bool m_focused;
	};
}