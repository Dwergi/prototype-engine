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
		ESCAPE = 256,
		ENTER = 257,
		TAB = 258,
		HOME = 268,
		END = 269,
		CAPS_LOCK = 280,
		PAUSE = 284,
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

	enum Modifiers : uint8
	{
		NONE = 0,
		SHIFT = 1 << 0,
		CTRL = 1 << 1,
		ALT = 1 << 2
	};

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

	struct InputBinding
	{
		int Key;
		uint8 Modifiers;
		InputAction Action;
	};

	struct IInputSource
	{
		virtual void UpdateInput() = 0;
		virtual MousePosition GetMousePosition() const = 0;
		virtual MousePosition GetScrollPosition() const = 0;

		virtual void CaptureMouse( bool capture ) = 0;
		virtual bool IsMouseCaptured() const = 0;

		virtual void GetKeyEvents( IArray<InputEvent>& out ) const = 0;
		virtual void GetMouseEvents( IArray<InputEvent>& out ) const = 0;

		virtual void BindKey( InputAction action, char ch, uint8 mods = 0 ) = 0;
		virtual void BindKey( InputAction action, Key key, uint8 mods = 0 ) = 0;
		virtual void BindMouseButton( InputAction action, MouseButton btn, uint8 mods = 0 ) = 0;
	};
}