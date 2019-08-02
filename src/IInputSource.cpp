#include "PCH.h"
#include "IInputSource.h"

namespace dd
{
	void IInputSource::UpdateInput()
	{
		OnUpdateInput();

		m_currentText = m_pendingText;
		m_pendingText.Clear();

		m_currentEvents = m_pendingEvents;
		m_pendingEvents.Clear();

		m_pendingMousePosition.Delta = m_pendingMousePosition.Absolute - m_currentMousePosition.Absolute;
		m_currentMousePosition = m_pendingMousePosition;

		m_pendingScrollPosition.Delta = m_pendingScrollPosition.Absolute - m_currentScrollPosition.Absolute;
		m_currentScrollPosition = m_pendingScrollPosition;
	}

	bool IInputSource::FindBinding(int key, uint8 modes, uint8 modifiers, InputBinding& binding) const
	{
		for (InputBinding& b : m_bindings)
		{
			if (b.Key == key && (b.Modes & modes) != 0)
			{
				binding = b;
				return true;
			}
		}

		return false;
	}

	bool IInputSource::IsBound(int key, uint8 modes, uint8 modifiers) const
	{
		InputBinding binding;
		return FindBinding(key, modes, modifiers, binding);
	}

	void IInputSource::BindKey(InputAction action, char ch, uint8 modes, uint8 modifiers)
	{
		BindKey(action, ( int) ch, modes, modifiers);
	}

	void IInputSource::BindKey(InputAction action, Key key, uint8 modes, uint8 modifiers)
	{
		DD_ASSERT(!IsBound(( int) key, modes, modifiers));

		InputBinding& binding = m_bindings.Allocate();
		binding.Key = ( int) key;
		binding.Action = action;
		binding.Modes = modes;
		binding.Modifiers = modifiers;
	}

	void IInputSource::BindMouseButton(InputAction action, MouseButton btn, uint8 modes, uint8 modifiers)
	{
		BindKey(action, (int) btn, modes, modifiers);
	}

	void IInputSource::GetKeyEvents(IArray<InputEvent>& events) const
	{
		events = m_currentKeyEvents;
	}

	void IInputSource::GetMouseEvents(IArray<InputEvent>& events) const
	{
		events = m_currentMouseEvents;
	}

	void IInputSource::SetMouseCapture(bool capture)
	{
		if (capture != m_mouseCaptured)
		{
			m_mouseCaptured = capture;

			OnSetMouseCapture(capture);
		}
	}

	void IInputSource::OnTextEvent(uint32 char_code)
	{
		m_pendingText.Add(char_code);
	}

	void IInputSource::OnKeyEvent(Key key, uint8 modifiers, InputType type)
	{
		InputBinding binding;
		if (FindBinding(key, m_mode, modifiers, binding))
		{
			m_pendingEvents.Add(InputEvent(binding.Action, type));
		}
	}

	void IInputSource::OnMouseButton(MouseButton btn, InputType type)
	{
		InputBinding binding;
		if (FindBinding((int) btn, m_mode, modifiers, binding))
		{
			m_pendingEvents.Add(InputEvent(binding.Action, type));
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
}