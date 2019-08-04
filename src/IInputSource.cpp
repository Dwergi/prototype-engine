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

		dd::InputMode* mode = dd::InputMode::Access(m_mode);

		if (mode->ShouldCentreMouse())
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

		dd::InputMode* mode = dd::InputMode::Access(m_mode);

		InputActionBinding binding;
		if (FindKeyBinding(key, modifiers, mode, binding))
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
	
	bool IInputSource::FindKeyBinding(Key key, uint8 modifiers, const InputMode* mode, InputActionBinding& out_binding) const
	{
		for (const InputActionBinding& b : m_bindings)
		{
			if (b.Key == key && b.Modifiers == modifiers && (mode == nullptr || (b.Modes & mode->ID()) != 0))
			{
				out_binding = b;
				return true;
			}
		}

		return false;
	}

	bool IInputSource::IsBound(Key key, uint8 modifiers, const InputMode* mode) const
	{
		InputActionBinding binding;
		return FindKeyBinding(key, modifiers, mode, binding);
	}

	void IInputSource::BindKeyInMode(const dd::InputMode& mode, InputAction action, Key key, uint8 modifiers)
	{
		DD_ASSERT(!IsBound(key, modifiers, &mode));

		InputActionBinding binding;
		binding.Key = key;
		binding.Action = action;
		binding.Modes |= mode.ID();
		binding.Modifiers = modifiers;
		m_bindings.push_back(binding);
	}

	void IInputSource::BindKey(InputAction action, Key key, uint8 modifiers)
	{
		DD_ASSERT(!IsBound(key, modifiers, nullptr));

		DD_TODO("This should also take the action type and resolve in the binding.");

		InputActionBinding binding;
		binding.Key = key;
		binding.Action = action;
		binding.Modes = dd::InputMode::ALL;
		binding.Modifiers = modifiers;
		m_bindings.push_back(binding);
	}
}