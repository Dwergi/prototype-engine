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
		InputKeyBindings();
		~InputKeyBindings();

		bool FindBinding(InputModeID mode, const InputEvent& evt, InputAction& out_action) const;

		void BindKey(dd::Key key, InputAction action);
		void BindKey(dd::Key key, InputAction action, std::string_view mode_name);
		void BindKey(dd::Key key, InputAction action, InputModeFlags modes);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action, std::string_view mode_name);
		void BindKey(dd::Key key, ModifierFlags modifiers, InputAction action, InputModeFlags modes);

		size_t Count() const { return m_bindings.size(); }

	private:

		// none, ctrl, shift, alt, alt + shift, alt + ctrl, ctrl + shift, alt + ctrl + shift
		const int NumModifiers = 8;

		struct KeyBinding
		{
			InputModeFlags Modes;
			Key Key;
			InputAction Actions[8] = { InputAction::NONE };
		};

		std::vector<KeyBinding> m_bindings;
	};
}
