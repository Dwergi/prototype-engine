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

		BindKeys();
	}

	void Input::Update(float delta_t)
	{
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
			if (evt.IsMouse() && m_mouseEnabled)
			{
				bound = FindKeyBinding(evt, action);
			}

			if (evt.IsKeyboard() && m_keyboardEnabled)
			{
				bound = FindKeyBinding(evt, action);
			}

			if (bound)
			{
				DispatchAction(action);
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

	void Input::BindKeys()
	{
		DD_TODO("Fix key binding!");
		//s_source->BindKey(InputAction::TOGGLE_DEBUG_UI, Key::ESCAPE);
		//s_source->BindKey(InputAction::TOGGLE_FREECAM, Key::F1);
		//s_source->BindKey(InputAction::TOGGLE_BOUNDS, Key::F2);
		//s_source->BindKey(InputAction::FORWARD, Key::W);
		//s_source->BindKey(InputAction::BACKWARD,Key::S);
		//s_source->BindKey(InputAction::LEFT,Key::A);
		//s_source->BindKey(InputAction::RIGHT,Key::D);
		//s_source->BindKey(InputAction::UP,Key::SPACE);
		//s_source->BindKey(InputAction::ADD_MINOR_TRAUMA,Key::R);
		//s_source->BindKey(InputAction::ADD_MAJOR_TRAUMA,Key::T);
		//s_source->BindKey(InputAction::DOWN, Key::LCTRL);
		//s_source->BindKey(InputAction::BOOST, Key::LSHIFT);
		//s_source->BindKey(InputAction::CAMERA_POS_1,Key::KEY_1);
		//s_source->BindKey(InputAction::CAMERA_POS_2,Key::KEY_2);
		//s_source->BindKey(InputAction::CAMERA_POS_3,Key::KEY_3);
		//s_source->BindKey(InputAction::CAMERA_POS_4,Key::KEY_4);
		//s_source->BindKey(InputAction::DECREASE_DEPTH, Key::HOME);
		//s_source->BindKey(InputAction::INCREASE_DEPTH, Key::END);
		//s_source->BindKey(InputAction::PAUSE, Key::PAUSE);
		//s_source->BindKey(InputAction::TIME_SCALE_DOWN, Key::PAGE_DOWN);
		//s_source->BindKey(InputAction::TIME_SCALE_UP, Key::PAGE_UP);

		//s_source->BindKeyInMode(*debug, InputAction::TOGGLE_ENTITY_DATA, Key::E);
		//s_source->BindKeyInMode(*game, InputAction::RESET_PHYSICS, Key::P);
		//s_source->BindKeyInMode(*debug, InputAction::TOGGLE_PICKING, Key::F3);
		//s_source->BindKeyInMode(*debug, InputAction::SELECT_MESH, Key::MOUSE_LEFT);
		//s_source->BindKeyInMode(*game, InputAction::SHOOT, Key::MOUSE_LEFT);
		//s_source->BindKey(InputAction::EXIT, Key::ESCAPE);

		//s_source->BindKey( InputAction::TOGGLE_CONSOLE, Key::F2, InputMode::DEBUG );
	}

	void Input::SetCurrentMode(std::string mode_name)
	{
		InputModeConfig* mode = InputModeConfig::Find(mode_name);
		DD_ASSERT(mode != nullptr);

		m_nextMode = mode->ID();
	}

	bool Input::FindKeyBinding(const InputEvent& evt, InputAction& out_action) const
	{
		if (m_bindings == nullptr)
		{
			return false;
		}
		return m_bindings->FindBinding(m_currentMode->ID(), evt, out_action);
	}

	void Input::RegisterActionHandler(InputAction action, InputHandler handler)
	{
		auto it = m_handlers.find(action);
		if (it == m_handlers.end())
		{
			auto result = m_handlers.insert(std::make_pair(action, std::vector<dd::InputHandler>()));
			it = result.first;
		}

		std::vector<dd::InputHandler>& handlers = it->second;
		handlers.push_back(handler);
	}

	void Input::DispatchAction(InputAction action) const
	{
		auto it = m_handlers.find(action);
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
