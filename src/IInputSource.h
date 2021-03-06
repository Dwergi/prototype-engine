//
// IInputSource.h - Interface for input source.
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

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

		bool GotMouseInput() const { return m_gotMouseInput; }

		IInputSource& SetMousePosition(glm::vec2 position);
		MousePosition GetMousePosition() const { return m_mousePosition.Read(); }

		MousePosition GetMouseScroll() const { return m_mouseScroll.Read(); }

		IInputSource& SetCaptureMouse(bool capture);
		bool IsMouseCaptured() const { return m_mouseCaptured; }

		IInputSource& SetCentreMouse(bool centre) { m_mouseRecenter = centre; return *this; }

		void GetEvents(IArray<InputEvent>& out) const { out = m_events.Read(); }

		const Array<uint32, InputEvent::MAX_EVENTS>& GetText() const { return m_text.Read(); }

	protected:
		
		// use these to register key events during OnUpdateInput
		void OnText(uint32 char_code);
		void OnKey(Key key, ModifierFlags modifiers, InputType action);
		void OnMousePosition(glm::vec2 absolute);
		void OnMouseWheel(glm::vec2 delta);

	private:

		DoubleBuffer<Array<InputEvent, InputEvent::MAX_EVENTS>> m_events;
		DoubleBuffer<Array<uint32, InputEvent::MAX_EVENTS>> m_text;
		DoubleBuffer<MousePosition> m_mousePosition;
		DoubleBuffer<MousePosition> m_mouseScroll;

		bool m_gotMouseInput { false };
		bool m_mouseCaptured { false };
		bool m_mouseRecenter { false };

		// implement these
		virtual void OnUpdateInput() = 0;
		virtual void OnSetMouseCapture(bool capture) = 0;
		virtual void OnSetMousePosition(glm::vec2 position) = 0;
	};
}