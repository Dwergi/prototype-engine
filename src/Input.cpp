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

static dd::Service<dd::IInputSource> s_source;
static dd::Service<dd::IWindow> s_window;

namespace dd
{
	Input::Input()
	{
	}

	void Input::Initialize()
	{
		InputModeConfig& game_mode = InputModeConfig::Create("game")
			.CaptureMouse(true)
			.CentreMouse(true)
			.ShowCursor(false);

		DD_ASSERT(game_mode.ID() == (1 << 0));

		InputModeConfig& debug_mode = InputModeConfig::Create("debug")
			.CaptureMouse(false)
			.CentreMouse(false)
			.ShowCursor(true);

		DD_ASSERT(debug_mode.ID() == (1 << 1));
	}

	void Input::Shutdown()
	{

	}

	void Input::Update(float delta_t)
	{
		DD_ASSERT(m_bindings != nullptr, "Key bindings still null in Input::Update!");

		// switch mode
		UpdateMode();
		
		bool should_capture = s_window->IsFocused() && m_currentMode != nullptr && m_currentMode->ShouldCaptureMouse();
		s_source->SetCaptureMouse(should_capture);

		s_source->UpdateInput();

		Array<InputEvent, 64> events;
		s_source->GetEvents(events);

		m_actions.clear();

		for (const InputEvent& evt : events)
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
				DispatchAction(action, evt.Type);
				
				if (UpdateHeldState(action, evt))
				{
					DispatchAction(action, InputType::Hold);
				}
			}

			m_actions.push_back(action);
		}
	}

	MousePosition Input::GetMousePosition() const
	{
		return s_source->GetMousePosition();
	}

	MousePosition Input::GetMouseScroll() const
	{
		return s_source->GetMousePosition();
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

			s_source->SetCentreMouse(next_mode->ShouldCentreMouse());
			s_window->SetCursor(next_mode->ShouldShowCursor() ? dd::Cursor::Arrow : dd::Cursor::Hidden);

			next_mode->ModeEntered();

			m_currentMode = next_mode;
		}
	}
}
