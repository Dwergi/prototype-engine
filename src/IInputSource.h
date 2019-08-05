//
// IInputSource.h - Interface for input source.
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "Array.h"
#include "DoubleBuffer.h"
#include "InputAction.h"
#include "InputEvent.h"
#include "InputMode.h"

namespace dd
{
	struct IInputSource
	{
		IInputSource();

		void UpdateInput();

		IInputSource& SetMousePosition(glm::vec2 position);
		MousePosition GetMousePosition() const { return m_mousePosition.Read(); }

		MousePosition GetMouseScroll() const { return m_mouseScroll.Read(); }

		IInputSource& SetCaptureMouse(bool capture);
		bool IsMouseCaptured() const { return m_mouseCaptured; }

		IInputSource& SetCentreMouse(bool centre) { m_mouseRecenter = true; }

		void GetEvents(IArray<InputEvent>& out) const { out = m_events.Read(); }

		virtual const char* GetClipboardText() const = 0;
		virtual void SetClipboardText(const char* text) = 0;

	protected:
		
		// use these to register key events during OnUpdateInput
		void OnText(uint32 char_code);
		void OnKey(Key key, ModifierFlags modifiers, InputType action);
		void OnMousePosition(glm::vec2 absolute);
		void OnMouseWheel(glm::vec2 absolute);

	private:
		static const int MAX_EVENTS = 32;

		DoubleBuffer<Array<InputEvent, MAX_EVENTS>> m_events;
		DoubleBuffer<Array<uint32, MAX_EVENTS>> m_text;
		DoubleBuffer<MousePosition> m_mousePosition;
		DoubleBuffer<MousePosition> m_mouseScroll;

		bool m_mouseCaptured { false };
		bool m_mouseRecenter { false };

		// implement these
		virtual void OnUpdateInput() = 0;
		virtual void OnSetMouseCapture(bool capture) = 0;
		virtual void OnSetMousePosition(glm::vec2 position) = 0;
	};
}