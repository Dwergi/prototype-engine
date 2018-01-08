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

	struct MousePosition
	{
		glm::vec2 Delta;
		glm::vec2 Absolute;

		MousePosition() {}
	};
	
	struct InputEvent
	{
		InputAction Action;
		InputType Type;
	};
	//===================================================================================

	class Input
	{
	public:

		// special keys that can't be typed as characters
		// these map to GLFW_KEY values
		enum class Key : int
		{
			ESCAPE = 256,
			ENTER = 257,
			TAB = 258,
			CAPS_LOCK = 280,
			F1 = 290,
			F2 = 291,
			LSHIFT = 340,
			LCTRL = 341,
			LALT = 342
		};

		enum class MouseButton : int
		{
			LEFT = 0,
			RIGHT = 1,
			MIDDLE = 2,
			BUTTON_4 = 3,
			BUTTON_5 = 4
		};

		typedef void( *KeyboardCallbackFunction )(GLFWwindow*, int, int, int, int);
		typedef void( *MouseButtonCallbackFunction )(GLFWwindow*, int, int, int);
		typedef void( *ScrollCallbackFunction )(GLFWwindow*, double, double);
		typedef void( *CharacterCallbackFunction )(GLFWwindow*, unsigned int);

		explicit Input( const Window& window );
		~Input();

		void Update( float delta_t );
		MousePosition GetMousePosition() const;
		MousePosition GetScrollPosition() const;

		void CaptureMouse( bool capture );
		bool IsMouseCaptured() const { return m_mouseCaptured; }

		// get all key events received in the last update
		// returns true if the output array successfully contained all of them
		// otherwise fills the array with Capacity() events and returns false
		bool GetKeyEvents( ArrayBase<InputEvent>& out ) const;

		void BindKey( char c, InputAction action );
		void BindKey( Key k, InputAction action );
		void BindMouseButton( MouseButton btn, InputAction action );

		// these shouldn't be used by default, mainly used by imgui
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
		MousePosition m_mousePosition;
		MousePosition m_scrollPosition;
		MousePosition m_tempScrollPosition;

		bool m_mouseCaptured;

		Vector<InputEvent> m_pendingEvents;
		Vector<InputEvent> m_currentEvents;

		Vector<KeyboardCallbackFunction> m_keyboardCallbacks;
		Vector<MouseButtonCallbackFunction> m_mouseButtonCallbacks;
		Vector<ScrollCallbackFunction> m_scrollCallbacks;
		Vector<CharacterCallbackFunction> m_charCallbacks;

		DenseMap<int, InputAction> m_bindings;

		static Input* m_pInstance;
		static void KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void CharCallback( GLFWwindow* window, unsigned int c );

		static InputType GetEventType( int action );
	};
}