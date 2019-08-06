//
// InputSystem.cpp
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#include "PCH.h"
#include "Input.h"

#include "DebugUI.h"
#include "IInputSource.h"
#include "InputKeyBindings.h"
#include "IWindow.h"

#include <imgui/imgui.h>

static dd::Service<dd::IWindow> s_window;

#pragma optimize("", off)

namespace dd
{
	Input::Input()
	{
	}

	void Input::Initialize()
	{
	}

	void Input::Shutdown()
	{
	}

	void Input::AddInputSource(IInputSource& source)
	{
		m_sources.push_back(&source);
	}

	void Input::Update(float delta_t)
	{
		DD_ASSERT(m_bindings != nullptr, "Key bindings not set!");

		// switch mode
		UpdateMode();
		DD_ASSERT(m_currentMode != nullptr, "Input mode not set!");

		m_actions.clear();

		dd::Array<InputEvent, 64> input_events;
		dd::Array<ActionKey, 64> dispatch_actions;

		for (IInputSource* source : m_sources)
		{
			input_events.Clear();

			bool should_capture = s_window->IsFocused() && m_currentMode->ShouldCaptureMouse();
			source->SetCaptureMouse(should_capture);

			source->UpdateInput();

			source->GetEvents(input_events);

			for (const InputEvent& evt : input_events)
			{
				bool bound = false;
				InputAction action;
				if ((evt.IsMouse() && m_mouseEnabled) ||
					(evt.IsKeyboard() && m_keyboardEnabled))
				{
					bound = m_bindings->FindBinding(m_currentMode->ID(), evt, action);
				}

				if (bound)
				{
					dispatch_actions.Add(ActionKey { action, evt.Type });

					if (UpdateHeldState(action, evt))
					{
						dispatch_actions.Add(ActionKey { action, InputType::Hold });
					}
				}

				m_actions.push_back(action);
			}
		}

		for (ActionKey& entry : dispatch_actions)
		{
			DispatchAction(entry.Action, entry.Type);
		}
	}

	MousePosition Input::GetMousePosition() const
	{
		DD_TODO("Maybe just cache last mouse position?");
		return m_sources[0]->GetMousePosition();
	}

	MousePosition Input::GetMouseScroll() const
	{
		DD_TODO("Maybe just cache last mouse position?");
		return m_sources[0]->GetMousePosition();
	}

	bool Input::GotInput(dd::InputAction action) const
	{
		auto it = std::find(m_actions.begin(), m_actions.end(), action);
		return it != m_actions.end();
	}

	bool Input::IsHeld(dd::InputAction action) const
	{
		auto it = m_held.find(action);
		if (it == m_held.end())
		{
			return false;
		}
		
		return it->second;
	}

	bool Input::UpdateHeldState(InputAction action, const InputEvent& evt)
	{
		if (evt.Type == InputType::Press)
		{
			m_held[action] = true;
			return true;
		}
		else if (evt.Type == InputType::Release)
		{
			m_held[action] = false;
			return false;
		}

		return false;
	}

	void Input::SetCurrentMode(std::string mode_name)
	{
		InputModeConfig* mode = InputModeConfig::Find(mode_name);
		DD_ASSERT(mode != nullptr);

		m_nextMode = mode->ID();
	}

	void Input::AddHandler(InputAction action, InputHandler handler)
	{
		AddHandler(action, InputType::Release, handler);
	}

	void Input::AddHandler(InputAction action, InputType type, InputHandler handler)
	{
		DD_TODO("Probably need a remove handler?");

		ActionKey key { action, type };

		auto it = m_handlers.find(key);
		if (it == m_handlers.end())
		{
			auto result = m_handlers.insert(std::make_pair(key, std::vector<dd::InputHandler>()));
			it = result.first;
		}

		std::vector<dd::InputHandler>& handlers = it->second;
		handlers.push_back(handler);
	}

	void Input::AddHeldHandler(InputAction action)
	{
		ActionKey key { action, InputType::Hold };

		auto it = m_handlers.find(key);
		if (it == m_handlers.end())
		{
			m_handlers.insert(std::make_pair(key, std::vector<dd::InputHandler>()));
		}
	}

	void Input::DispatchAction(InputAction action, InputType type) const
	{
		ActionKey key { action, type };

		auto it = m_handlers.find(key);
		if (it == m_handlers.end())
		{
			return;
		}

		const std::vector<dd::InputHandler>& handlers = it->second;
		for (const dd::InputHandler& handler : handlers)
		{
			handler();
		}
	}

	void Input::UpdateMode()
	{
		InputModeConfig* next_mode = InputModeConfig::Access(m_nextMode);
		if (next_mode != nullptr && next_mode != m_currentMode)
		{
			if (m_currentMode != nullptr)
			{
				m_currentMode->ModeExited();
			}

			for (IInputSource* source : m_sources)
			{
				source->SetCentreMouse(next_mode->ShouldCentreMouse());
			}

			s_window->SetCursor(next_mode->ShouldShowCursor() ? dd::Cursor::Arrow : dd::Cursor::Hidden);

			next_mode->ModeEntered();

			m_currentMode = next_mode;
		}
	}
}
