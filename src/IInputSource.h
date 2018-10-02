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
		ESCAPE		= 256,
		ENTER		= 257,
		TAB			= 258,
		HOME		= 268,
		END			= 269,
		CAPS_LOCK	= 280,
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
		LALT		= 342
	};

	enum class MouseButton : int
	{
		LEFT		= 0,
		RIGHT		= 1,
		MIDDLE		= 2,
		BUTTON_4	= 3,
		BUTTON_5	= 4
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
		virtual void UpdateInput() = 0;
		virtual MousePosition GetMousePosition() const = 0;
		virtual MousePosition GetScrollPosition() const = 0;

		virtual void CaptureMouse( bool capture ) = 0;
		virtual bool IsMouseCaptured() const = 0;

		virtual void GetKeyEvents( IArray<InputEvent>& out ) const = 0;
		virtual void GetMouseEvents( IArray<InputEvent>& out ) const = 0;

		virtual void BindKey( InputAction action, char ch, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) = 0;
		virtual void BindKey( InputAction action, Key key, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) = 0;
		virtual void BindMouseButton( InputAction action, MouseButton btn, uint8 modes = InputMode::ALL, uint8 modifiers = 0 ) = 0;

		virtual void SetMode( uint8 mode ) = 0;
		virtual uint8 GetMode() const = 0;
	};
}