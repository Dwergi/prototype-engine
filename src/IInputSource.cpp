#include "PCH.h"
#include "IInputSource.h"

#include "IWindow.h"

static dd::Service<dd::IWindow> s_window;

namespace dd
{
	void IInputSource::UpdateInput()
	{
		OnUpdateInput();

		m_currentText = m_pendingText;
		m_pendingText.Clear();

		m_currentEvents = m_pendingEvents;
		m_pendingEvents.Clear();

		if (m_mouseCaptured)
		{
			glm::ivec2 window_size = s_window->GetSize();
			glm::vec2 window_center(window_size.x / 2, window_size.y / 2);

			m_pendingMousePosition.Delta = window_center - m_pendingMousePosition.Absolute;
			
			// recenter the mouse
			SetMousePosition(window_center);
		}
		else
		{
			m_pendingMousePosition.Delta = m_pendingMousePosition.Absolute - m_currentMousePosition.Absolute;
		}
		m_currentMousePosition = m_pendingMousePosition;

		m_pendingScrollPosition.Delta = m_pendingScrollPosition.Absolute - m_currentScrollPosition.Absolute;
		m_currentScrollPosition = m_pendingScrollPosition;
	}

	IInputSource& IInputSource::SetMousePosition(glm::vec2 position)
	{
		OnSetMousePosition(position);
		return *this;
	}

	IInputSource& IInputSource::SetMouseCapture(bool capture)
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
		m_pendingText.Add(char_code);
	}

	void IInputSource::OnKey(Key key, uint8 modifiers, InputType type)
	{
		if (key == Key::NONE)
		{
			return;
		}

		InputActionBinding binding;
		if (FindBinding(key, m_mode, modifiers, binding))
		{
			InputEvent evt;
			evt.Key = key;
			evt.Modifiers = modifiers;
			evt.Action = binding.Action;
			evt.Type = type;
			evt.IsMouse = key > Key::LAST_KEY;
			m_pendingEvents.Add(evt);
		}
	}

	void IInputSource::OnMousePosition(glm::vec2 absolute)
	{
		m_pendingMousePosition.Absolute = absolute;
	}

	void IInputSource::OnMouseWheel(glm::vec2 absolute)
	{
		m_pendingScrollPosition.Absolute = absolute;
	}

	bool IInputSource::FindBinding(Key key, uint8 modes, uint8 modifiers, InputActionBinding& binding) const
	{
		for (InputActionBinding& b : m_bindings)
		{
			if (b.Key == key && (b.Modes & modes) != 0)
			{
				binding = b;
				return true;
			}
		}

		return false;
	}

	bool IInputSource::IsBound(Key key, uint8 modes, uint8 modifiers) const
	{
		InputActionBinding binding;
		return FindBinding(key, modes, modifiers, binding);
	}

	void IInputSource::BindKey(InputAction action, Key key, uint8 modes, uint8 modifiers)
	{
		DD_ASSERT(!IsBound(key, modes, modifiers));

		InputActionBinding& binding = m_bindings.Allocate();
		binding.Key = key;
		binding.Action = action;
		binding.Modes = modes;
		binding.Modifiers = modifiers;
	}
}