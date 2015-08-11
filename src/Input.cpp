//
// Input.cpp - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Input.h"

#include "Window.h"

#include "GLFW/glfw3.h"

dd::Input* dd::Input::m_pInstance = nullptr;

dd::Input::Input( const dd::Window& window )
{
	ASSERT( m_pInstance == nullptr );

	m_pInstance = this;

	m_glfwWindow = window.GetInternalWindow();
	glfwSetKeyCallback( m_glfwWindow, &KeyboardCallback );

	m_bindings.Register( GLFW_KEY_W, dd::InputAction::FORWARD );
	m_bindings.Register( GLFW_KEY_S, dd::InputAction::BACKWARD );
	m_bindings.Register( GLFW_KEY_A, dd::InputAction::LEFT );
	m_bindings.Register( GLFW_KEY_D, dd::InputAction::RIGHT );
}

dd::Input::~Input()
{
	ASSERT( m_pInstance == this );

	m_pInstance = nullptr;
	glfwSetKeyCallback( m_glfwWindow, nullptr );
}

void dd::Input::KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	InputType event_type = GetEventType( action );
	InputAction event_action = m_pInstance->m_bindings.GetAction( key );

	if( event_type != InputType::NONE && event_action != InputAction::NONE )
	{
		InputEvent& new_event = m_pInstance->m_pendingEvents.Allocate();
		new_event.Action = event_action;
		new_event.Type = event_type;
	}
}

void dd::Input::Update()
{
	double newX, newY;

	glfwGetCursorPos( m_glfwWindow, &newX, &newY );

	m_currentMousePosition.DeltaX = m_currentMousePosition.X - newX;
	m_currentMousePosition.DeltaY = m_currentMousePosition.Y - newY;
	m_currentMousePosition.X = newX;
	m_currentMousePosition.Y = newY;

	m_currentEvents.Swap( m_pendingEvents );
	m_pendingEvents.Clear();
}

void dd::Input::GetKeyEvents( dd::StackArray<InputEvent, 64>& events ) const
{
	for( uint i = 0; i < m_currentEvents.Size(); ++i )
	{
		events.Push( m_currentEvents[ i ] );
	}
}

dd::InputType dd::Input::GetEventType( int action )
{
	if( action == GLFW_PRESS )
		return dd::InputType::PRESSED;

	if( action == GLFW_RELEASE )
		return dd::InputType::RELEASED;

	return dd::InputType::NONE;
}

dd::MousePosition dd::Input::GetMousePosition() const
{
	return m_currentMousePosition;
}

void dd::InputBindings::Register( int key, InputAction action )
{
	m_bindings.Add( key, action );
}

dd::InputAction dd::InputBindings::GetAction( int key ) const
{
	if( !m_bindings.Contains( key ) )
		return dd::InputAction::NONE;

	return m_bindings[ key ];
}