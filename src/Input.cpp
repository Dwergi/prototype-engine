//
// Input.cpp - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Input.h"

#include "Window.h"

#include "GLFW/glfw3.h"

namespace dd
{
	Input* Input::m_pInstance = nullptr;

	Input::Input( const Window& window )
	{
		ASSERT( m_pInstance == nullptr );

		m_pInstance = this;

		m_glfwWindow = window.GetInternalWindow();
		glfwSetKeyCallback( m_glfwWindow, &KeyboardCallback );
		glfwSetMouseButtonCallback( m_glfwWindow, &MouseButtonCallback );
		glfwSetScrollCallback( m_glfwWindow, &ScrollCallback );
		glfwSetCharCallback( m_glfwWindow, &CharCallback );

		m_bindings.Register( GLFW_KEY_W, InputAction::FORWARD );
		m_bindings.Register( GLFW_KEY_S, InputAction::BACKWARD );
		m_bindings.Register( GLFW_KEY_A, InputAction::LEFT );
		m_bindings.Register( GLFW_KEY_D, InputAction::RIGHT );
	}

	Input::~Input()
	{
		ASSERT( m_pInstance == this );

		m_pInstance = nullptr;
		glfwSetKeyCallback( m_glfwWindow, nullptr );
	}

	void Input::KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		InputType event_type = GetEventType( action );
		InputAction event_action = m_pInstance->m_bindings.GetAction( key );

		if( event_type != InputType::NONE && event_action != InputAction::NONE )
		{
			InputEvent& new_event = m_pInstance->m_pendingEvents.Allocate();
			new_event.Action = event_action;
			new_event.Type = event_type;
		}

		for( GLFWkeyfun fn : m_pInstance->m_keyboardCallbacks )
		{
			(*fn)( window, key, scancode, action, mods );
		}
	}

	void Input::MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
	{
		for( GLFWmousebuttonfun fn : m_pInstance->m_mouseButtonCallbacks )
		{
			(*fn)( window, button, action, mods );
		}
	}

	void Input::ScrollCallback( GLFWwindow* window, double xoffset, double yoffset )
	{
		for( GLFWscrollfun fn : m_pInstance->m_scrollCallbacks )
		{
			(*fn)( window, xoffset, yoffset );
		}
	}

	void Input::CharCallback( GLFWwindow* window, unsigned int c )
	{
		for( GLFWcharfun fn : m_pInstance->m_charCallbacks )
		{
			(*fn)( window, c );
		}
	}

	void Input::AddKeyboardCallback( GLFWkeyfun cb )
	{
		m_keyboardCallbacks.Add( cb );
	}

	void Input::AddMouseCallback( GLFWmousebuttonfun cb )
	{
		m_mouseButtonCallbacks.Add( cb );
	}

	void Input::AddScrollCallback( GLFWscrollfun cb )
	{
		m_scrollCallbacks.Add( cb );
	}

	void Input::AddCharCallback( GLFWcharfun cb )
	{
		m_charCallbacks.Add( cb );
	}

	void Input::RemoveKeyboardCallback( GLFWkeyfun cb )
	{
		m_keyboardCallbacks.RemoveItem( cb );
	}

	void Input::RemoveMouseCallback( GLFWmousebuttonfun cb )
	{
		m_mouseButtonCallbacks.RemoveItem( cb );
	}

	void Input::RemoveScrollCallback( GLFWscrollfun cb )
	{
		m_scrollCallbacks.RemoveItem( cb );
	}

	void Input::RemoveCharCallback( GLFWcharfun cb )
	{
		m_charCallbacks.RemoveItem( cb );
	}

	void Input::Update()
	{
		double newX, newY;

		glfwGetCursorPos( m_glfwWindow, &newX, &newY );

		m_currentMousePosition.DeltaX = (float) (m_currentMousePosition.X - newX);
		m_currentMousePosition.DeltaY = (float) (m_currentMousePosition.Y - newY);
		m_currentMousePosition.X = (float) newX;
		m_currentMousePosition.Y = (float) newY;

		m_currentEvents.Swap( m_pendingEvents );
		m_pendingEvents.Clear();
	}

	void Input::GetKeyEvents( Array<InputEvent, 64>& events ) const
	{
		for( uint i = 0; i < m_currentEvents.Size(); ++i )
		{
			events.Push( m_currentEvents[i] );
		}
	}

	InputType Input::GetEventType( int action )
	{
		if( action == GLFW_PRESS )
			return InputType::PRESSED;

		if( action == GLFW_RELEASE )
			return InputType::RELEASED;

		return InputType::NONE;
	}

	MousePosition Input::GetMousePosition() const
	{
		return m_currentMousePosition;
	}

	void InputBindings::Register( int key, InputAction action )
	{
		m_bindings.Add( key, action );
	}

	InputAction InputBindings::GetAction( int key ) const
	{
		if( !m_bindings.Contains( key ) )
			return InputAction::NONE;

		return m_bindings[key];
	}
}