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

		void BindKey(std::string mode_name, const InputEvent& evt, InputAction action);
		void BindKey(InputModeFlags modes, const InputEvent& evt, InputAction action);

	private:

		struct KeyBinding
		{
			dd::InputModeFlags Modes;
			dd::InputEvent Event;
			dd::InputAction Action;
		};
		
		std::vector<KeyBinding> m_bindings;
	};
}
