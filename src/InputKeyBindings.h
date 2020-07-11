//
// InputKeyBindings.h - Handler for binding actions to functions.
// Dispatches actions to registered functions.
// Copyright (C) Sebastian Nordgren 
// April 12th 2016
//

#pragma once

#include "IInputSource.h"

namespace dd
{
	struct InputKeyBindings
	{
		InputKeyBindings(std::string name);
		~InputKeyBindings();

		bool FindBinding(InputModeID mode, const InputEvent& evt, InputAction& out_action) const;

		void BindKey(dd::Key key, InputAction action);
		void BindKey(dd::Key key, InputAction action, std::string_view mode_name);
		void BindKey(dd::Key key, InputAction action, InputModeFlags modes);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action, std::string_view mode_name);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action, InputModeFlags modes);

	private:

		struct KeyBinding
		{
			dd::InputModeFlags Modes;
			dd::Key Key;
			dd::ModifierFlags Modifiers;
			dd::InputAction Action;
		};
		
		std::string m_name;
		std::vector<KeyBinding> m_bindings;
	};
}
