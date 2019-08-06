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
		bool IsHeld(dd::InputAction action) const;

		// Get ALL actions that occurred last frame. Probably better to use GotInput().
		const std::vector<dd::InputAction>& GetInputs() const { return m_actions; }
		
		MousePosition GetMousePosition() const;
		MousePosition GetMouseScroll() const;
		
		// Add a handler, assumes Release as type.
		void AddHandler(dd::InputAction action, dd::InputHandler handler);
		void AddHandler(dd::InputAction action, dd::InputType type, dd::InputHandler handler);

		// Just mark the given action as being handled by IsHeld().
		void AddHeldHandler(dd::InputAction action);

		void SetCurrentMode(std::string mode_name);
		std::string GetCurrentMode() const { return m_currentMode != nullptr ? m_currentMode->GetName() : std::string(); }

		void EnableMouse(bool enabled) { m_mouseEnabled = enabled; }
		void EnableKeyboard(bool enabled) { m_keyboardEnabled = enabled; }

		void SetKeyBindings(const InputKeyBindings& bindings) { m_bindings = &bindings; }

	private:

		struct ActionKey
		{
			dd::InputAction Action;
			dd::InputType Type;
		};

		struct ActionKeyHash { std::size_t operator()(const ActionKey& key) const { return ((size_t) key.Action << 32) | ((size_t) key.Type); } };
		struct ActionKeyEqual { std::size_t operator()(const ActionKey& a, const ActionKey& b) const { return a.Action == b.Action && a.Type == b.Type; } };

		// All actions that were triggered last frame.
		std::vector<InputAction> m_actions;

		std::unordered_map<ActionKey, std::vector<InputHandler>, ActionKeyHash, ActionKeyEqual> m_handlers;
		std::unordered_map<InputAction, bool> m_held;

		const InputKeyBindings* m_bindings { nullptr };
		InputModeConfig* m_currentMode { nullptr };
		InputModeID m_nextMode { 0 };

		bool m_mouseEnabled { true };
		bool m_keyboardEnabled { true };

		void DispatchAction(InputAction action, InputType type) const;

		void UpdateMode();

		// Update map of held actions, returning true if action is currently held.
		bool UpdateHeldState(InputAction action, const InputEvent& evt);
	};
}