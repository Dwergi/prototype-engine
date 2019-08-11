#include "PCH.h"
#include "IInputSource.h"

#include "IWindow.h"

static dd::Service<dd::IWindow> s_window;

namespace dd
{
	IInputSource::IInputSource() :
		m_events(new Array<InputEvent, MAX_EVENTS>(), new Array<InputEvent, MAX_EVENTS>()),
		m_text(new Array<uint32, MAX_EVENTS>(), new Array<uint32, MAX_EVENTS>()),
		m_mousePosition(new MousePosition(), new MousePosition()),
		m_mouseScroll(new MousePosition(), new MousePosition())
	{

	}

	void IInputSource::UpdateInput()
	{
		m_gotMouseInput = false;

		OnUpdateInput();

		m_events.Swap();
		m_events.Write().Clear();

		m_text.Swap();
		m_text.Write().Clear();

		if (m_mouseRecenter)
		{
			glm::ivec2 window_size = s_window->GetSize();
			glm::vec2 window_center(window_size.x / 2, window_size.y / 2);

			m_mousePosition.Write().Delta = window_center - m_mousePosition.Write().Absolute;
			
			SetMousePosition(window_center);
		}
		else
		{
			m_mousePosition.Write().Delta = m_mousePosition.Write().Absolute - m_mousePosition.Read().Absolute;
		}
		m_mousePosition.Swap();
		m_mousePosition.Duplicate();
		m_mousePosition.Write().Delta = glm::vec2(0);

		m_mouseScroll.Write().Delta = m_mouseScroll.Write().Absolute - m_mouseScroll.Read().Absolute;
		m_mouseScroll.Swap();
		m_mouseScroll.Duplicate();
		m_mouseScroll.Write().Delta = glm::vec2(0);
	}

	IInputSource& IInputSource::SetMousePosition(glm::vec2 position)
	{
		OnSetMousePosition(position);
		return *this;
	}

	IInputSource& IInputSource::SetCaptureMouse(bool capture)
	{
		if (capture != m_mouseCaptured)
		{
			m_mouseCaptured = capture;

			OnSetMouseCapture(capture);
		}

		return *this;
	}

	void IInputSource::OnText(uint32 char_code)
	{
		m_text.Write().Add(char_code);
	}

	void IInputSource::OnKey(Key key, ModifierFlags modifiers, InputType type)
	{
		if (key == Key::NONE)
		{
			return;
		}

		if (key > Key::KB_LAST)
		{
			m_gotMouseInput = true;
		}

		InputEvent evt(key, modifiers, type);
		m_events.Write().Add(evt);
	}

	void IInputSource::OnMousePosition(glm::vec2 absolute)
	{
		m_gotMouseInput = true;

		m_mousePosition.Write().Absolute = absolute;
	}

	void IInputSource::OnMouseWheel(glm::vec2 delta)
	{
		m_gotMouseInput = true;

		m_mouseScroll.Write().Delta = delta;
		m_mouseScroll.Write().Absolute += delta;
	}
}