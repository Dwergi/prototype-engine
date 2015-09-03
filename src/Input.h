//
// Input.h - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "DenseMap.h"
#include "Array.h"
#include "Vector.h"

#include "InputAction.h"

struct GLFWwindow;

namespace dd
{
	class Window;

	class InputBindings
	{
	public:

		void Register( int key, InputAction action );

		InputAction GetAction( int key ) const;
		
	private:

		DenseMap<int, InputAction> m_bindings;
	};

	struct MousePosition
	{
		double DeltaX;
		double DeltaY;
		double X;
		double Y;
	};

	struct InputEvent
	{
		InputAction Action;
		InputType Type;
	};

	class Input
	{
	public:
		Input( const Window& window );
		~Input();

		void Update();
		MousePosition GetMousePosition() const;
		void GetKeyEvents( Array<InputEvent, 64>& out ) const;

	private:

		GLFWwindow* m_glfwWindow;
		MousePosition m_currentMousePosition;

		Vector<InputEvent> m_pendingEvents;
		Vector<InputEvent> m_currentEvents;

		InputBindings m_bindings;

		static Input* m_pInstance;
		static void KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods );

		static InputType GetEventType( int action );
	};
}