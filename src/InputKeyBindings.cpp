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
		DD_ASSERT(evt.Type != InputType::None);

		for (const KeyBinding& binding : m_bindings)
		{
			if (binding.Modes.Has(mode) && binding.Key == evt.Key && 
				(binding.Modifiers == ModifierFlags(Modifier::None) || binding.Modifiers == evt.Modifiers))
			{
				out_action = binding.Action;
				return true;
			}
		}

		return false;
	}

	void InputKeyBindings::BindKey(Key key, InputAction action)
	{
		InputModeFlags modes;
		modes.Fill();
		BindKey(key, ModifierFlags(), action, modes);
	}

	void InputKeyBindings::BindKey(Key key, InputAction action, std::string mode_name)
	{
		BindKey(key, ModifierFlags(), action, mode_name);
	}

	void InputKeyBindings::BindKey(Key key, InputAction action, InputModeFlags modes)
	{
		BindKey(key, ModifierFlags(), action, modes);
	}

	void InputKeyBindings::BindKey(Key key, ModifierFlags modifiers, InputAction action, std::string mode_name)
	{
		InputModeConfig* mode = InputModeConfig::Find(mode_name);
		DD_ASSERT(mode != nullptr, "Mode '%s' not registered!", mode_name.c_str());

		BindKey(key, modifiers, action, mode->ID());
	}

	void InputKeyBindings::BindKey(Key key, ModifierFlags modifiers, InputAction action, InputModeFlags modes)
	{
		for (KeyBinding& binding : m_bindings)
		{
			if (binding.Modes.HasAny(modes) && binding.Key == key && binding.Modifiers == modifiers)
			{
				DD_ASSERT(binding.Action == action);
				binding.Modes.SetAll(modes);
				return;
			}
		}

		KeyBinding new_binding;
		new_binding.Modes = modes;
		new_binding.Key = key;
		new_binding.Modifiers = modifiers;
		new_binding.Action = action;
		m_bindings.push_back(new_binding);

		std::sort(m_bindings.begin(), m_bindings.end(), [](const KeyBinding& a, const KeyBinding& b)
		{
			return a.Key < b.Key;
		});
	}
}