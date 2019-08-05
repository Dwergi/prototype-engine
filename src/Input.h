//
// InputSystem.h
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "InputAction.h"
#include "InputEvent.h"
#include "InputKeys.h"
#include "InputMode.h"

namespace dd
{
	struct InputKeyBindings;

	using InputHandler = std::function<void()>;

	struct Input
	{
		Input();

		void Initialize();
		void Shutdown();

		void Update(float delta_t);

		// Did the given input occur in the last frame?
		bool GotInput(dd::InputAction action) const;

		// Get ALL actions that occurred last frame. Probably better to use GotInput().
		const std::vector<dd::InputAction> GetInputs() const { return m_actions; }
		
		MousePosition GetMousePosition() const;
		MousePosition GetMouseScroll() const;

		void RegisterActionHandler(dd::InputAction action, dd::InputHandler handler);

		void SetCurrentMode(std::string mode_name);

		void EnableMouse(bool enabled) { m_mouseEnabled = enabled; }
		void EnableKeyboard(bool enabled) { m_keyboardEnabled = enabled; }

	private:

		// All actions that were triggered last frame.
		std::vector<dd::InputAction> m_actions;

		std::unordered_map<dd::InputAction, std::vector<dd::InputHandler>> m_handlers;

		const InputKeyBindings* m_bindings { nullptr };
		InputModeConfig* m_currentMode { nullptr };
		InputModeID m_nextMode { 0 };

		bool m_mouseEnabled { true };
		bool m_keyboardEnabled { true };

		void BindKeys();

		bool FindKeyBinding(const InputEvent& evt, InputAction& binding) const;
		void DispatchAction(InputAction action) const;

		void UpdateMode();
	};
}