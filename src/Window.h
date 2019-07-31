//
// Window.h - Wrapper around GLFW windowing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

struct GLFWwindow;

namespace dd
{
	struct Window
	{
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
		void ShowConsole(bool visible);

		void Swap();

		glm::ivec2 GetSize() const { return m_size; }
		int GetWidth() const { return m_size.x; }
		int GetHeight() const { return m_size.y; }

		float GetAspectRatio() const { return float( m_size.x ) / float( m_size.y ); }

		GLFWwindow* GetInternalWindow() const { return m_glfwWindow; }	

	private:
		static Window* m_pInstance;

		GLFWwindow* m_glfwWindow { nullptr };
		String32 m_title;

		bool m_focused { false };

		glm::ivec2 m_size;

		static void OnFramebufferResize( GLFWwindow* window, int width, int height );
		static void OnWindowResize( GLFWwindow* window, int width, int height );
		static void OnError( int error, const char* message );
	};
}