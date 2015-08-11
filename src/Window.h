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

		bool IsValid() const;
		bool ShouldClose() const;

		void MakeBorderless();

		void Swap();

		GLFWwindow* GetInternalWindow() const { return m_glfwWindow; }

	private:
		GLFWwindow* m_glfwWindow;
		dd::String  m_title;
		int m_sizeX, m_sizeY;
	};
}