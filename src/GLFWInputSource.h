//
// Input.h - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "DenseMap.h"
#include "IInputSource.h"
#include "Vector.h"

struct GLFWwindow;

namespace dd
{
	class Window;

	struct GLFWInputSource : IInputSource
	{
		typedef void( *KeyboardCallbackFunction )(GLFWwindow*, int, int, int, int);
		typedef void( *MouseButtonCallbackFunction )(GLFWwindow*, int, int, int);
		typedef void( *ScrollCallbackFunction )(GLFWwindow*, double, double);
		typedef void( *CharacterCallbackFunction )(GLFWwindow*, unsigned int);

		explicit GLFWInputSource( const Window& window );
		~GLFWInputSource();

		virtual void UpdateInput() override;
		virtual MousePosition GetMousePosition() const override;
		virtual MousePosition GetScrollPosition() const override;

		virtual void CaptureMouse( bool capture ) override;
		virtual bool IsMouseCaptured() const override { return m_mouseCaptured; }

		virtual void GetKeyEvents( IArray<InputEvent>& out ) const override;
		virtual void GetMouseEvents( IArray<InputEvent>& out ) const override;

		virtual void BindKey( char c, InputAction action ) override;
		virtual void BindKey( Key k, InputAction action ) override;
		virtual void BindMouseButton( MouseButton btn, InputAction action ) override;

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

		static const int MAX_EVENTS = 32;

		Array<InputEvent, MAX_EVENTS> m_pendingKeyEvents;
		Array<InputEvent, MAX_EVENTS> m_currentKeyEvents;

		Array<InputEvent, MAX_EVENTS> m_pendingMouseEvents;
		Array<InputEvent, MAX_EVENTS> m_currentMouseEvents;

		Vector<KeyboardCallbackFunction> m_keyboardCallbacks;
		Vector<MouseButtonCallbackFunction> m_mouseButtonCallbacks;
		Vector<ScrollCallbackFunction> m_scrollCallbacks;
		Vector<CharacterCallbackFunction> m_charCallbacks;

		DenseMap<int, InputAction> m_bindings;

		static GLFWInputSource* m_pInstance;
		static void KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void CharCallback( GLFWwindow* window, unsigned int c );

		static InputType GetEventType( int action );

		bool IsBound( int action ) const;
	};
}