//
// InputSystem.h
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "InputAction.h"
#include "InputEvent.h"
#include "InputKeyBindings.h"
#include "InputKeys.h"
#include "InputMode.h"

namespace dd
{
	struct InputKeyBindings;
	struct IInputSource;

	using InputHandler = std::function<void()>;

	struct InputReceived
	{
		InputAction Action;
		InputType Type;

		bool operator==(const InputReceived& other) const { return Action == other.Action && Type == other.Type; }
	};

	struct Input
	{
		Input();

		void Initialize();
		void Shutdown();

		void AddInputSource(IInputSource& source);

		void Update(float delta_t);

		// Did the given input occur in the last frame? If type is not given, assumes InputType::Release.
		bool GotInput(InputAction action) const;
		bool GotInput(InputAction action, InputType type) const;
		bool IsHeld(InputAction action) const;
		
		MousePosition GetMousePosition() const;
		MousePosition GetMouseScroll() const;
		
		// Add a handler, assumes Release as type.
		void AddHandler(InputAction action, InputHandler handler);
		void AddHandler(InputAction action, InputType type, InputHandler handler);

		// Just mark the given action as being handled by IsHeld(). Requires manual checking of IsHeld().
		void AddHeldHandler(InputAction action);

		void SetCurrentMode(std::string_view mode_name);
		std::string GetCurrentMode() const { return m_currentMode != nullptr ? m_currentMode->GetName() : std::string(); }

		void EnableMouse(bool enabled) { m_mouseEnabled = enabled; }
		void EnableKeyboard(bool enabled) { m_keyboardEnabled = enabled; }

		Array<uint32, InputEvent::MAX_EVENTS> GetText() const { return m_text; }

	private:

		struct InputRecvHash { std::size_t operator()(const InputReceived& key) const { return ((size_t) key.Action << 32) | ((size_t) key.Type); } };

		// All actions that were triggered last frame.
		std::vector<InputReceived> m_actions;
		std::vector<IInputSource*> m_sources;

		std::unordered_map<InputReceived, std::vector<InputHandler>, InputRecvHash> m_handlers;
		std::unordered_map<InputAction, bool> m_held;

		InputModeConfig* m_currentMode { nullptr };
		InputModeID m_nextMode { 0 };

		Array<uint32, InputEvent::MAX_EVENTS> m_text;

		MousePosition m_mousePosition;
		MousePosition m_mouseScroll;

		bool m_mouseEnabled { true };
		bool m_keyboardEnabled { true };

		void DispatchAction(InputAction action, InputType type) const;

		void UpdateMode();

		// Update map of held actions, returning true if action is currently held.
		bool UpdateHeldState(InputAction action, const InputEvent& evt);
	};
}
