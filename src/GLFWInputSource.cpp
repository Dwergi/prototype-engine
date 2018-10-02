//
// Input.cpp - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "GLFWInputSource.h"

#include "Window.h"

#include "GLFW/glfw3.h"

namespace dd
{
	GLFWInputSource* GLFWInputSource::m_pInstance = nullptr;

	GLFWInputSource::GLFWInputSource( const Window& window )
	{
		DD_ASSERT( m_pInstance == nullptr );

		m_pInstance = this;

		m_glfwWindow = window.GetInternalWindow();
		glfwSetKeyCallback( m_glfwWindow, &KeyboardCallback );
		glfwSetMouseButtonCallback( m_glfwWindow, &MouseButtonCallback );
		glfwSetScrollCallback( m_glfwWindow, &ScrollCallback );
		glfwSetCharCallback( m_glfwWindow, &CharCallback );
	}

	GLFWInputSource::~GLFWInputSource()
	{
		DD_ASSERT( m_pInstance == this );

		m_pInstance = nullptr;
		glfwSetKeyCallback( m_glfwWindow, nullptr );
	}

	bool GLFWInputSource::FindBinding( int key, uint8 modes, uint8 modifiers, InputBinding& binding ) const
	{
		for( InputBinding& b : m_bindings )
		{
			if( b.Key == key && (b.Modes & modes) != 0 )
			{
				binding = b;
				return true;
			}
		}

		return false;
	}

	bool GLFWInputSource::IsBound( int key, uint8 modes, uint8 modifiers ) const
	{
		InputBinding binding;
		return FindBinding( key, modes, modifiers, binding );
	}

	void GLFWInputSource::BindKey( InputAction action, char ch, uint8 modes, uint8 modifiers )
	{
		DD_ASSERT( !IsBound( (int) ch, modes, modifiers ) );

		InputBinding& binding	= m_bindings.Allocate();
		binding.Key				= (int) ch;
		binding.Action			= action;
		binding.Modes			= modes;
		binding.Modifiers		= modifiers;
	}

	void GLFWInputSource::BindKey( InputAction action, Key key, uint8 modes, uint8 modifiers )
	{
		DD_ASSERT( !IsBound( (int) key, modes, modifiers ) );

		InputBinding& binding	= m_bindings.Allocate();
		binding.Key				= (int) key;
		binding.Action			= action;
		binding.Modes			= modes;
		binding.Modifiers		= modifiers;
	}

	void GLFWInputSource::BindMouseButton( InputAction action, MouseButton btn, uint8 modes, uint8 modifiers )
	{
		DD_ASSERT( !IsBound( (int) btn, modes, modifiers ) );
		
		InputBinding& binding	= m_bindings.Allocate();
		binding.Key				= (int) btn;
		binding.Action			= action;
		binding.Modes			= modes;
		binding.Modifiers		= modifiers;
	}

	void GLFWInputSource::UpdateInput()
	{
		double newX, newY;

		glfwGetCursorPos( m_glfwWindow, &newX, &newY );

		m_mousePosition.Delta = glm::vec2( (float) (m_mousePosition.Absolute.x - newX), (float) (m_mousePosition.Absolute.y - newY) );
		m_mousePosition.Absolute = glm::vec2( (float) newX, (float) newY );

		m_scrollPosition = m_tempScrollPosition;
		m_tempScrollPosition.Delta = glm::vec2();

		m_currentKeyEvents = m_pendingKeyEvents;
		m_pendingKeyEvents.Clear();

		m_currentMouseEvents = m_pendingMouseEvents;
		m_pendingMouseEvents.Clear();
	}

	void GLFWInputSource::GetKeyEvents( IArray<InputEvent>& events ) const
	{
		events = m_currentKeyEvents;
	}

	void GLFWInputSource::GetMouseEvents( IArray<InputEvent>& events ) const
	{
		events = m_currentMouseEvents;
	}

	InputType GLFWInputSource::GetEventType( int action )
	{
		if( action == GLFW_PRESS )
			return InputType::PRESSED;

		if( action == GLFW_RELEASE )
			return InputType::RELEASED;

		return InputType::NONE;
	}

	MousePosition GLFWInputSource::GetMousePosition() const
	{
		return m_mousePosition;
	}

	MousePosition GLFWInputSource::GetScrollPosition() const
	{
		return m_scrollPosition;
	}

	void GLFWInputSource::CaptureMouse( bool capture )
	{
		if( m_mouseCaptured != capture )
		{
			m_mouseCaptured = capture;
			glfwSetInputMode( m_glfwWindow, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
		}
	}

	void GLFWInputSource::KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
	{
		InputBinding binding;
		if( m_pInstance->FindBinding( key, m_pInstance->m_mode, mods, binding ) )
		{
			InputEvent new_event;
			new_event.Action = binding.Action;
			new_event.Type = GetEventType( action );
			m_pInstance->m_pendingKeyEvents.Add( new_event );
		}

		// we didn't handle it, maybe someone else will
		for( GLFWkeyfun fn : m_pInstance->m_keyboardCallbacks )
		{
			(*fn)(window, key, scancode, action, mods);
		}
	}

	void GLFWInputSource::MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
	{
		InputBinding binding;
		if( m_pInstance->FindBinding( action, m_pInstance->m_mode, mods, binding ) )
		{
			InputEvent new_event;
			new_event.Action = binding.Action;
			new_event.Type = GetEventType( action );
			m_pInstance->m_pendingMouseEvents.Add( new_event );
		}

		for( GLFWmousebuttonfun fn : m_pInstance->m_mouseButtonCallbacks )
		{
			(*fn)(window, button, action, mods);
		}
	}

	void GLFWInputSource::ScrollCallback( GLFWwindow* window, double xOffset, double yOffset )
	{
		m_pInstance->m_tempScrollPosition.Delta = glm::vec2( (float) xOffset, (float) yOffset );
		m_pInstance->m_tempScrollPosition.Absolute += glm::vec2( (float) xOffset, (float) yOffset );

		for( GLFWscrollfun fn : m_pInstance->m_scrollCallbacks )
		{
			(*fn)(window, xOffset, yOffset);
		}
	}

	void GLFWInputSource::CharCallback( GLFWwindow* window, unsigned int c )
	{
		for( GLFWcharfun fn : m_pInstance->m_charCallbacks )
		{
			(*fn)(window, c);
		}
	}

	void GLFWInputSource::AddKeyboardCallback( KeyboardCallbackFunction cb )
	{
		m_keyboardCallbacks.Add( cb );
	}

	void GLFWInputSource::AddMouseCallback( MouseButtonCallbackFunction cb )
	{
		m_mouseButtonCallbacks.Add( cb );
	}

	void GLFWInputSource::AddScrollCallback( ScrollCallbackFunction cb )
	{
		m_scrollCallbacks.Add( cb );
	}

	void GLFWInputSource::AddCharCallback( CharacterCallbackFunction cb )
	{
		m_charCallbacks.Add( cb );
	}

	void GLFWInputSource::RemoveKeyboardCallback( KeyboardCallbackFunction cb )
	{
		m_keyboardCallbacks.RemoveItem( cb );
	}

	void GLFWInputSource::RemoveMouseCallback( MouseButtonCallbackFunction cb )
	{
		m_mouseButtonCallbacks.RemoveItem( cb );
	}

	void GLFWInputSource::RemoveScrollCallback( ScrollCallbackFunction cb )
	{
		m_scrollCallbacks.RemoveItem( cb );
	}

	void GLFWInputSource::RemoveCharCallback( CharacterCallbackFunction cb )
	{
		m_charCallbacks.RemoveItem( cb );
	}
}