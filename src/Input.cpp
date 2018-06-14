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
		DD_ASSERT( m_pInstance == nullptr );

		m_pInstance = this;

		m_mouseCaptured = false;

		m_glfwWindow = window.GetInternalWindow();
		glfwSetKeyCallback( m_glfwWindow, &KeyboardCallback );
		glfwSetMouseButtonCallback( m_glfwWindow, &MouseButtonCallback );
		glfwSetScrollCallback( m_glfwWindow, &ScrollCallback );
		glfwSetCharCallback( m_glfwWindow, &CharCallback );
	}

	Input::~Input()
	{
		DD_ASSERT( m_pInstance == this );

		m_pInstance = nullptr;
		glfwSetKeyCallback( m_glfwWindow, nullptr );
	}

	void Input::BindKey( char c, InputAction action )
	{
		m_bindings.Add( (int) c, action );
	}

	void Input::BindKey( Key k, InputAction action )
	{
		m_bindings.Add( (int) k, action );
	}

	void Input::BindMouseButton( MouseButton btn, InputAction action )
	{
		m_bindings.Add( (int) btn, action );
	}

	void Input::Update( float delta_t )
	{
		double newX, newY;

		glfwGetCursorPos( m_glfwWindow, &newX, &newY );

		m_mousePosition.Delta = glm::vec2( (float) (m_mousePosition.Absolute.x - newX), (float) (m_mousePosition.Absolute.y - newY) );
		m_mousePosition.Absolute = glm::vec2( (float) newX, (float) newY );

		m_scrollPosition = m_tempScrollPosition;
		m_tempScrollPosition.Delta = glm::vec2();

		m_currentEvents.Swap( m_pendingEvents );
		m_pendingEvents.Clear();
	}

	bool Input::GetKeyEvents( IArray<InputEvent>& events ) const
	{
		for( int i = 0; i < m_currentEvents.Size(); ++i )
		{
			if( events.Size() == events.Capacity() )
				return false;

			events.Push( m_currentEvents[i] );
		}

		return true;
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
		return m_mousePosition;
	}

	MousePosition Input::GetScrollPosition() const
	{
		return m_scrollPosition;
	}

	void Input::CaptureMouse( bool capture )
	{
		if( m_mouseCaptured != capture )
		{
			m_mouseCaptured = capture;
			glfwSetInputMode( m_glfwWindow, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
		}
	}

	void Input::KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		InputType event_type = GetEventType( action );
		InputAction event_action = m_pInstance->m_bindings.Contains( key ) ? m_pInstance->m_bindings[key] : InputAction::NONE;

		if( event_type != InputType::NONE && event_action != InputAction::NONE )
		{
			InputEvent& new_event = m_pInstance->m_pendingEvents.Allocate();
			new_event.Action = event_action;
			new_event.Type = event_type;
		}

		// we didn't handle it, maybe someone else will
		for( GLFWkeyfun fn : m_pInstance->m_keyboardCallbacks )
		{
			(*fn)(window, key, scancode, action, mods);
		}
	}

	void Input::MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
	{
		InputType event_type = GetEventType( action );
		InputAction event_action = m_pInstance->m_bindings.Contains( button ) ? m_pInstance->m_bindings[button] : InputAction::NONE;

		if( event_type != InputType::NONE && event_action != InputAction::NONE )
		{
			InputEvent& new_event = m_pInstance->m_pendingEvents.Allocate();
			new_event.Action = event_action;
			new_event.Type = event_type;
		}

		for( GLFWmousebuttonfun fn : m_pInstance->m_mouseButtonCallbacks )
		{
			(*fn)(window, button, action, mods);
		}
	}

	void Input::ScrollCallback( GLFWwindow* window, double xOffset, double yOffset )
	{
		m_pInstance->m_tempScrollPosition.Delta = glm::vec2( (float) xOffset, (float) yOffset );
		m_pInstance->m_tempScrollPosition.Absolute += glm::vec2( (float) xOffset, (float) yOffset );

		for( GLFWscrollfun fn : m_pInstance->m_scrollCallbacks )
		{
			(*fn)(window, xOffset, yOffset);
		}
	}

	void Input::CharCallback( GLFWwindow* window, unsigned int c )
	{
		for( GLFWcharfun fn : m_pInstance->m_charCallbacks )
		{
			(*fn)(window, c);
		}
	}

	void Input::AddKeyboardCallback( KeyboardCallbackFunction cb )
	{
		m_keyboardCallbacks.Add( cb );
	}

	void Input::AddMouseCallback( MouseButtonCallbackFunction cb )
	{
		m_mouseButtonCallbacks.Add( cb );
	}

	void Input::AddScrollCallback( ScrollCallbackFunction cb )
	{
		m_scrollCallbacks.Add( cb );
	}

	void Input::AddCharCallback( CharacterCallbackFunction cb )
	{
		m_charCallbacks.Add( cb );
	}

	void Input::RemoveKeyboardCallback( KeyboardCallbackFunction cb )
	{
		m_keyboardCallbacks.RemoveItem( cb );
	}

	void Input::RemoveMouseCallback( MouseButtonCallbackFunction cb )
	{
		m_mouseButtonCallbacks.RemoveItem( cb );
	}

	void Input::RemoveScrollCallback( ScrollCallbackFunction cb )
	{
		m_scrollCallbacks.RemoveItem( cb );
	}

	void Input::RemoveCharCallback( CharacterCallbackFunction cb )
	{
		m_charCallbacks.RemoveItem( cb );
	}
}