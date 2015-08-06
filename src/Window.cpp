//
// Window.cpp - Wrapper around GLFW windowing.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Window.h"

#include "GLFW/glfw3.h"

dd::Window::Window( int resX, int resY, const dd::StringBase& title )
	: m_title( title )
{
	if( !glfwInit() )
		return;

	m_glfwWindow = glfwCreateWindow( resX, resY, m_title.c_str(), NULL, NULL);

	if( m_glfwWindow != nullptr )
	{
		glfwMakeContextCurrent( m_glfwWindow );
	}
}

dd::Window::~Window()
{
	glfwTerminate();
}

void dd::Window::MakeBorderless()
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

bool dd::Window::IsValid() const
{
	return m_glfwWindow != nullptr;
}

bool dd::Window::ShouldClose() const
{
	return glfwWindowShouldClose( m_glfwWindow ) != 0;
}

void dd::Window::Close()
{
	glfwDestroyWindow( m_glfwWindow );
	m_glfwWindow = nullptr;
}

void dd::Window::Swap()
{
	glfwSwapBuffers( m_glfwWindow );
	glfwPollEvents();
}