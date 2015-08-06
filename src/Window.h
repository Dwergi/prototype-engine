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

		Window( int resX, int resY, const dd::StringBase& title );
		~Window();

		void Close();

		bool IsValid() const;
		bool ShouldClose() const;

		void MakeBorderless();

		void Swap();

		GLFWwindow* GetInternalWindow() const { return m_glfwWindow; }

	private:
		GLFWwindow* m_glfwWindow;
		dd::String<32> m_title;
	};
}