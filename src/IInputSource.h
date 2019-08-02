//
// IInputSource.h - Interface for input source.
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "Array.h"
#include "InputAction.h"

namespace dd
{
	// special keys that can't be typed as characters
	// these map to GLFW_KEY values
	enum class Key : int
	{
		NONE		= -1,
		ESCAPE		= 256,
		ENTER		= 257,
		TAB			= 258,
		PAGE_UP		= 266,
		PAGE_DOWN	= 267,
		HOME		= 268,
		END			= 269,
		PAUSE		= 284,
		F1			= 290,
		F2			= 291,
		F3			= 292,
		F4			= 293,
		F5			= 294,
		F6			= 295,
		F7			= 296,
		F8			= 297,
		F9			= 298,
		F10			= 299,
		LSHIFT		= 340,
		LCTRL		= 341,
		LALT		= 342,

		LAST		= 512
	};

	enum class MouseButton : int
	{
		LEFT = (int) Key::LAST,
		RIGHT,
		MIDDLE,
		BUTTON_4,
		BUTTON_5,
	};

	enum Modifiers : uint8
	{
		NONE		= 0,
		SHIFT		= 1 << 0,
		CTRL		= 1 << 1,
		ALT			= 1 << 2
	};

	struct MousePosition
	{
		glm::vec2 Delta;
		glm::vec2 Absolute;
	};

	struct InputEvent
	{
		InputAction Action;
		InputType Type;
		bool IsMouse { false };
	};

	struct InputBinding
	{
		int Key;
		uint8 Modes;
		uint8 Modifiers;
		InputAction Action;
	};

	struct InputMode
	{
		enum : uint8
		{
			NONE	= 0,
			GAME	= 1 << 0,
			DEBUG	= 1 << 1,
			ALL		= 0xFF
		};
	};

	struct IInputSource
	{
		void UpdateInput();
		MousePosition GetMousePosition() const { return m_currentMousePosition; }
		MousePosition GetScrollPosition() const { return m_currentScrollPosition; }

		virtual void SetMouseCapture(bool capture);
		bool IsMouseCaptured() const { return m_mouseCaptured; }

		void GetEvents(IArray<InputEvent>& out) const;

		void BindKey(InputAction action, char ch, uint8 modes = InputMode::ALL, uint8 modifiers = 0);
		void BindKey(InputAction action, Key key, uint8 modes = InputMode::ALL, uint8 modifiers = 0);
		void BindMouseButton(InputAction action, MouseButton btn, uint8 modes = InputMode::ALL, uint8 modifiers = 0);

		void SetMode(uint8 mode) { m_mode = mode; }
		uint8 GetMode() const { return m_mode; }

	protected:
		
		// use these to register key events during OnUpdateInput
		void OnTextEvent(uint32 char_code);
		void OnKeyEvent(Key key, uint8 modifiers, InputType action);
		void OnMouseButton(MouseButton btn, InputType action);
		void OnMousePosition(glm::vec2 absolute);
		void OnMouseWheel(glm::vec2 absolute);

	private:
		static const int MAX_EVENTS = 32;
		Array<InputEvent, MAX_EVENTS> m_pendingEvents;
		Array<InputEvent, MAX_EVENTS> m_currentEvents;

		Array<uint32, MAX_EVENTS> m_currentText;
		Array<uint32, MAX_EVENTS> m_pendingText;

		MousePosition m_pendingMousePosition;
		MousePosition m_currentMousePosition;
		MousePosition m_pendingScrollPosition;
		MousePosition m_currentScrollPosition;

		uint8 m_mode { InputMode::NONE };

		Vector<InputBinding> m_bindings;
		bool m_mouseCaptured { false };

		bool IsBound(int key, uint8 modes, uint8 modifiers) const;
		bool FindBinding(int key, uint8 modes, uint8 modifiers, InputBinding& binding) const;

		// implement these
		virtual void OnUpdateInput() = 0;
		virtual void OnSetMouseCapture(bool capture) = 0;

		virtual const char* GetClipboardText() const = 0;
		virtual void SetClipboardText(void* data, const char* text) = 0;
	};
}