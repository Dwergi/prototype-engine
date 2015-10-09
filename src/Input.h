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
	//===================================================================================

	struct MousePosition
	{
		float DeltaX;
		float DeltaY;
		float X;
		float Y;
	};
	//===================================================================================

	struct InputEvent
	{
		InputAction Action;
		InputType Type;
	};
	//===================================================================================

	class Input
	{
	public:

		typedef void( *KeyboardCallbackFunction )(GLFWwindow*, int, int, int, int);
		typedef void( *MouseButtonCallbackFunction )(GLFWwindow*, int, int, int);
		typedef void( *ScrollCallbackFunction )(GLFWwindow*, double, double);
		typedef void( *CharacterCallbackFunction )(GLFWwindow*, unsigned int);

		Input( const Window& window );
		~Input();

		void Update();
		MousePosition GetMousePosition() const;
		void GetKeyEvents( Array<InputEvent, 64>& out ) const;

		void AddKeyboardCallback( KeyboardCallbackFunction cb );
		void AddMouseCallback( MouseButtonCallbackFunction cb );
		void AddScrollCallback( ScrollCallbackFunction cb );
		void AddCharCallback( CharacterCallbackFunction cb );

		void RemoveKeyboardCallback( KeyboardCallbackFunction cb );
		void RemoveMouseCallback( MouseButtonCallbackFunction cb );
		void RemoveScrollCallback( ScrollCallbackFunction cb );
		void RemoveCharCallback( CharacterCallbackFunction cb );

	private:

		GLFWwindow* m_glfwWindow;
		MousePosition m_currentMousePosition;

		Vector<InputEvent> m_pendingEvents;
		Vector<InputEvent> m_currentEvents;

		Vector<KeyboardCallbackFunction> m_keyboardCallbacks;
		Vector<MouseButtonCallbackFunction> m_mouseButtonCallbacks;
		Vector<ScrollCallbackFunction> m_scrollCallbacks;
		Vector<CharacterCallbackFunction> m_charCallbacks;

		InputBindings m_bindings;

		static Input* m_pInstance;
		static void KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void CharCallback( GLFWwindow* window, unsigned int c );

		static InputType GetEventType( int action );
	};
}