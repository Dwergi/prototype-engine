//
// Window.cpp - Wrapper around GLFW windowing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Window.h"

#include "GLError.h"

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

namespace dd
{
	Window* Window::m_pInstance = nullptr;

	Window::Window( glm::ivec2 resolution, const char* title )
		: m_title( title ),
		m_size( resolution )
	{
		DD_ASSERT( m_pInstance == nullptr );
		m_pInstance = this;

		if( !glfwInit() )
			return;

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

		m_glfwWindow = glfwCreateWindow( m_size.x, m_size.y, m_title.c_str(), NULL, NULL);
		if( m_glfwWindow != nullptr )
		{
			glfwMakeContextCurrent( m_glfwWindow );

			DD_ASSERT( IsContextValid() );
		}

		if( gl3wInit() || !gl3wIsSupported( 4, 5 ) )
			throw std::exception( "Failed to create gl3w context!" );

		glfwSwapInterval( 0 );
		glfwSetFramebufferSizeCallback( m_glfwWindow, OnFramebufferResize );
		glfwSetWindowSizeCallback( m_glfwWindow, OnWindowResize );
		glfwSetErrorCallback( OnError );

		Swap();
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	void Window::OnWindowResize( GLFWwindow* window, int width, int height )
	{
		m_pInstance->Resize( width, height );
	}

	void Window::OnFramebufferResize( GLFWwindow* window, int width, int height )
	{
		m_pInstance->m_size = glm::ivec2( width, height );
	}

	void Window::OnError( int error, const char* description )
	{
		DD_ASSERT( false, description );
	}

	void Window::Resize( int resX, int resY )
	{
		glfwSetWindowSize( m_glfwWindow, resX, resY );
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

	void Window::SetToClose() const
	{
		glfwSetWindowShouldClose( m_glfwWindow, true );
	}

	bool Window::IsFocused() const
	{
		return m_focused;
	}

	void Window::Close()
	{
		glfwDestroyWindow( m_glfwWindow );
		m_glfwWindow = nullptr;
	}

	void Window::Swap()
	{
		glfwPollEvents();
		glfwSwapBuffers( m_glfwWindow );
		CheckOGLError();

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		CheckOGLError();

		glViewport( 0, 0, m_size.x, m_size.y );
		CheckOGLError();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		m_focused = glfwGetWindowAttrib( m_glfwWindow, GLFW_FOCUSED ) != 0;
	}

	bool Window::IsContextValid() const
	{
		return glfwGetCurrentContext() == m_glfwWindow;
	}
}