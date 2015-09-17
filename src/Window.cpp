//
// Window.cpp - Wrapper around GLFW windowing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Window.h"

#include "GLFW/glfw3.h"

namespace dd
{
	Window::Window( int resX, int resY, const char* title )
		: m_title( title ),
		m_sizeX( resX ),
		m_sizeY( resY )
	{
		if( !glfwInit() )
			return;

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

		m_glfwWindow = glfwCreateWindow( m_sizeX, m_sizeY, m_title.c_str(), NULL, NULL);

		if( m_glfwWindow != nullptr )
		{
			glfwMakeContextCurrent( m_glfwWindow );
		}
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	void Window::Resize( int resX, int resY )
	{
		m_sizeX = resX;
		m_sizeY = resY;

		glfwSetWindowSize( m_glfwWindow, m_sizeX, m_sizeY );
	}

	void Window::MakeBorderless()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode( monitor );

		glfwWindowHint( GLFW_RED_BITS, mode->redBits );
		glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
		glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
		glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );

		if( m_glfwWindow != nullptr )
		{
			Close();
		}

		m_glfwWindow = glfwCreateWindow( mode->width, mode->height, m_title.c_str(), monitor, NULL );
	}

	bool Window::IsValid() const
	{
		return m_glfwWindow != nullptr;
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose( m_glfwWindow ) != 0;
	}

	void Window::Close()
	{
		glfwDestroyWindow( m_glfwWindow );
		m_glfwWindow = nullptr;
	}

	void Window::Swap()
	{
		glViewport( 0, 0, m_sizeX, m_sizeY );
		glClearColor( 0, 0, 0, 1 );
		glClear( GL_COLOR_BUFFER_BIT );

		glfwSwapBuffers( m_glfwWindow );
		glfwPollEvents();
	}
}