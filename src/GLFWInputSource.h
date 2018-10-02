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

		virtual void BindKey( InputAction action, char ch, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) override;
		virtual void BindKey( InputAction action, Key key, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) override;
		virtual void BindMouseButton( InputAction action, MouseButton btn, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) override;

		virtual void SetMode( uint8 mode ) override { m_mode = mode; }
		virtual uint8 GetMode() const override { return m_mode; }

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

		GLFWwindow* m_glfwWindow { nullptr };
		MousePosition m_mousePosition;
		MousePosition m_scrollPosition;
		MousePosition m_tempScrollPosition;
		uint8 m_mode { InputMode::NONE };

		bool m_mouseCaptured { false };

		static const int MAX_EVENTS = 32;

		Array<InputEvent, MAX_EVENTS> m_pendingKeyEvents;
		Array<InputEvent, MAX_EVENTS> m_currentKeyEvents;

		Array<InputEvent, MAX_EVENTS> m_pendingMouseEvents;
		Array<InputEvent, MAX_EVENTS> m_currentMouseEvents;

		Vector<KeyboardCallbackFunction> m_keyboardCallbacks;
		Vector<MouseButtonCallbackFunction> m_mouseButtonCallbacks;
		Vector<ScrollCallbackFunction> m_scrollCallbacks;
		Vector<CharacterCallbackFunction> m_charCallbacks;

		Vector<InputBinding> m_bindings;

		static GLFWInputSource* m_pInstance;
		static void KeyboardCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
		static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
		static void CharCallback( GLFWwindow* window, unsigned int c );

		static InputType GetEventType( int action );

		bool IsBound( int key, uint8 modes, uint8 modifiers ) const;
		bool FindBinding( int key, uint8 modes, uint8 modifiers, InputBinding& binding ) const;
	};
}