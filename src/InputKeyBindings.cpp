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

		// ctrl, shift, alt, alt + shift, alt + ctrl, ctrl + shift, alt + ctrl + shift
		dd::Array<const KeyBinding*, 7> matches;

		for (const KeyBinding& binding : m_bindings)
		{
			if (binding.Modes.Has(mode) &&
				binding.Key == evt.Key)
			{
				matches.Add(&binding);
			}
		}

		// check for exact modifier match, ie. Shift+A
		for (const KeyBinding* match : matches)
		{
			if (match->Modifiers == evt.Modifiers)
			{
				out_action = match->Action;
				return true;
			}
		}

		// check for inexact match, ie. A
		for (const KeyBinding* match : matches)
		{
			if (match->Modifiers == Modifier::None)
			{
				out_action = match->Action;
				return true;
			}
		}

		return false;
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::InputAction action)
	{
		dd::InputModeFlags modes;
		modes.Fill();
		BindKey(key, dd::ModifierFlags(), action, modes);
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::InputAction action, std::string_view mode_name)
	{
		BindKey(key, dd::ModifierFlags(), action, mode_name);
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::InputAction action, dd::InputModeFlags modes)
	{
		BindKey(key, dd::ModifierFlags(), action, modes);
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::ModifierFlags modifiers, dd::InputAction action)
	{
		dd::InputModeFlags modes;
		modes.Fill();
		BindKey(key, modifiers, action, modes);
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::ModifierFlags modifiers, dd::InputAction action, std::string_view mode_name)
	{
		dd::InputModeConfig* mode = dd::InputModeConfig::Find(mode_name);
		DD_ASSERT(mode != nullptr, "Mode '%s' not registered!", std::string(mode_name).c_str());

		BindKey(key, modifiers, action, mode->ID());
	}

	void InputKeyBindings::BindKey(dd::Key key, dd::ModifierFlags modifiers, dd::InputAction action, InputModeFlags modes)
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