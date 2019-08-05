//
// InputKeyBindings.h - Handler for binding actions to functions.
// Dispatches actions to registered functions.
// Copyright (C) Sebastian Nordgren 
// April 12th 2016
//

#include "PCH.h"
#include "InputKeyBindings.h"

namespace dd
{
	InputKeyBindings::InputKeyBindings()
	{
	}

	InputKeyBindings::~InputKeyBindings()
	{
	}

	bool InputKeyBindings::FindBinding(InputModeID mode, const InputEvent& evt, InputAction& out_action) const
	{
		for (const KeyBinding& binding : m_bindings)
		{
			if (binding.Modes.Has(mode) && binding.Event == evt)
			{
				out_action = binding.Action;
				return true;
			}
		}

		return false;
	}

	void InputKeyBindings::BindKey(std::string mode_name, const InputEvent& evt, InputAction action)
	{
		InputModeConfig* mode = InputModeConfig::Find(mode_name);
		DD_ASSERT(mode != nullptr, "Mode '%s' not registered!", mode_name.c_str());

		BindKey(mode->ID(), evt, action);
	}

	void InputKeyBindings::BindKey(InputModeFlags modes, const InputEvent& evt, InputAction action)
	{
		for (const KeyBinding& binding : m_bindings)
		{
			if (binding.Event == evt && binding.Modes.HasAny(modes))
			{
				DD_ASSERT(binding.Action == action);
				binding.Modes.SetAll(modes);
				return;
			}
		}

		KeyBinding new_binding;
		new_binding.Modes = modes;
		new_binding.Event = evt;
		new_binding.Action = action;
		m_bindings.push_back(new_binding);

		std::sort(m_bindings.begin(), m_bindings.end(), [](const KeyBinding& a, const KeyBinding& b)
			{
				return a.Event.Key < b.Event.Key
			});
	}
}