//
// IInputSource.h - Interface for input source.
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "Array.h"
#include "InputAction.h"
#include "InputMode.h"

namespace dd
{
	// special keys that can't be typed as characters
	// these map to GLFW_KEY values
	enum class Key : int16
	{
		NONE		= -1,
		SPACE		= 32,
		KEY_0		= 48,
		KEY_1		= 49,
		KEY_2		= 50,
		KEY_3		= 51,
		KEY_4		= 52,
		KEY_5		= 53,
		KEY_6		= 54,
		KEY_7		= 55,
		KEY_8		= 56,
		KEY_9		= 57,
		A			= 65,
		B			= 66,
		C			= 67,
		D			= 68,
		E			= 69,
		F			= 70,
		G			= 71,
		H			= 72,
		I			= 73,
		J			= 74,
		K			= 75,
		L			= 76,
		M			= 77,
		N			= 78,
		O			= 79,
		P			= 80,
		Q			= 81,
		R			= 82,
		S			= 83,
		T			= 84,
		U			= 85,
		V			= 86,
		W			= 87,
		X			= 88,
		Y			= 89,
		Z			= 90,
		ESCAPE		= 256,
		ENTER		= 257,
		TAB			= 258,
		BACKSPACE	= 259,
		INSERT		= 260,
		DEL			= 261,
		RIGHT_ARROW	= 262,
		LEFT_ARROW	= 263,
		DOWN_ARROW	= 264,
		UP_ARROW	= 265,
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

		LAST_KEY	= 512,

		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_4,
		MOUSE_5
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
		glm::vec2 Delta { 0, 0 };
		glm::vec2 Absolute { 0, 0 };
	};

	struct InputEvent
	{
		Key Key { Key::NONE };
		uint8 Modifiers { 0 };
		InputAction Action { InputAction::NONE };
		InputType Type { InputType::NONE };
		bool IsMouse { false };
	};

	struct InputActionBinding
	{
		Key Key { Key::NONE };
		uint8 Modes { 0 };
		uint8 Modifiers { 0 };
		InputAction Action { InputAction::NONE };
	};

	struct IInputSource
	{
		void UpdateInput();

		IInputSource& SetMousePosition(glm::vec2 position);
		MousePosition GetMousePosition() const { return m_currentMousePosition; }

		MousePosition GetMouseScroll() const { return m_currentScrollPosition; }

		IInputSource& SetMouseCapture(bool capture);
		bool IsMouseCaptured() const { return m_mouseCaptured; }

		void GetEvents(IArray<InputEvent>& out) const { out = m_currentEvents; }

		void BindKey(InputAction action, Key key, uint8 modes = InputMode::ALL, uint8 modifiers = 0);

		virtual const char* GetClipboardText() const = 0;
		virtual void SetClipboardText(const char* text) = 0;

	protected:
		
		// use these to register key events during OnUpdateInput
		void OnText(uint32 char_code);
		void OnKey(Key key, uint8 modifiers, InputType action);
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

		Vector<InputActionBinding> m_bindings;
		bool m_mouseCaptured { false };

		bool IsBound(Key key, uint8 modes, uint8 modifiers) const;
		bool FindBinding(Key key, uint8 modes, uint8 modifiers, InputActionBinding& binding) const;

		// implement these
		virtual void OnUpdateInput() = 0;
		virtual void OnSetMouseCapture(bool capture) = 0;
		virtual void OnSetMousePosition(glm::vec2 position) = 0;
	};
}