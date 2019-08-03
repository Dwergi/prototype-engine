//
// InputSystem.cpp
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#include "PCH.h"
#include "InputSystem.h"

#include "DebugUI.h"
#include "IInputSource.h"
#include "InputBindings.h"
#include "IWindow.h"

#include <imgui/imgui.h>

static dd::Service<dd::IInputSource> s_source;
static dd::Service<dd::IWindow> s_window;

namespace dd
{
	InputSystem::InputSystem()
	{
	}

	void InputSystem::Initialize()
	{
		InputMode& game_mode = InputMode::Create("game")
			.CaptureMouse(true)
			.ShowCursor(false);

		DD_ASSERT(game_mode.ID() == InputMode::GAME);

		InputMode& debug_mode = InputMode::Create("debug")
			.CaptureMouse(false)
			.ShowCursor(true);

		DD_ASSERT(debug_mode.ID() == InputMode::DEBUG);

		BindKeys();
	}

	void InputSystem::Update(float delta_t)
	{
		InputMode* current_mode = nullptr;
		
		if (m_currentMode != InputMode::NONE)
		{
			current_mode = &InputMode::Access(m_currentMode);
		}

		if (m_nextMode != m_currentMode)
		{
			InputMode& next_mode = InputMode::Access(m_nextMode);
			if (current_mode != nullptr)
			{
				current_mode->ModeExited();
			}

			s_source->SetMouseCapture(next_mode.ShouldCaptureMouse());
			s_window->SetCursor(next_mode.ShouldShowCursor() ? dd::Cursor::Arrow : dd::Cursor::Hidden);

			next_mode.ModeEntered();
			 
			current_mode = &next_mode;
			m_currentMode = m_nextMode;
		}
		
		bool should_capture = s_window->IsFocused() && current_mode != nullptr && current_mode->ShouldCaptureMouse();
		s_source->SetMouseCapture(should_capture);

		s_source->UpdateInput();

		Array<InputEvent, 64> events;
		s_source->GetEvents(events);

		// update input
		for (const InputEvent& evt : events)
		{
			if (!evt.IsMouse && !ImGui::GetIO().WantCaptureKeyboard)
			{
				m_bindings->Dispatch(evt);
			}

			if (evt.IsMouse && !ImGui::GetIO().WantCaptureMouse)
			{
				m_bindings->Dispatch(evt);
			}
		}
	}

	void InputSystem::BindKeys()
	{
		s_source->BindKey(InputAction::TOGGLE_DEBUG_UI, Key::ESCAPE);
		s_source->BindKey(InputAction::TOGGLE_FREECAM, Key::F1);
		s_source->BindKey(InputAction::TOGGLE_BOUNDS, Key::F2);
		s_source->BindKey(InputAction::TOGGLE_PICKING, Key::F3, InputMode::DEBUG);
		s_source->BindKey(InputAction::FORWARD, Key::W);
		s_source->BindKey(InputAction::BACKWARD,Key::S);
		s_source->BindKey(InputAction::LEFT,Key::A);
		s_source->BindKey(InputAction::RIGHT,Key::D);
		s_source->BindKey(InputAction::UP,Key::SPACE);
		s_source->BindKey(InputAction::ADD_MINOR_TRAUMA,Key::R);
		s_source->BindKey(InputAction::ADD_MAJOR_TRAUMA,Key::T);
		s_source->BindKey(InputAction::DOWN, Key::LCTRL);
		s_source->BindKey(InputAction::BOOST, Key::LSHIFT);
		s_source->BindKey(InputAction::TOGGLE_ENTITY_DATA,Key::E, InputMode::DEBUG);
		s_source->BindKey(InputAction::CAMERA_POS_1,Key::KEY_1);
		s_source->BindKey(InputAction::CAMERA_POS_2,Key::KEY_2);
		s_source->BindKey(InputAction::CAMERA_POS_3,Key::KEY_3);
		s_source->BindKey(InputAction::CAMERA_POS_4,Key::KEY_4);
		s_source->BindKey(InputAction::DECREASE_DEPTH, Key::HOME);
		s_source->BindKey(InputAction::INCREASE_DEPTH, Key::END);
		s_source->BindKey(InputAction::RESET_PHYSICS,Key::P, InputMode::GAME);
		s_source->BindKey(InputAction::TOGGLE_PICKING,Key::P, InputMode::DEBUG);
		s_source->BindKey(InputAction::PAUSE, Key::PAUSE);
		s_source->BindKey(InputAction::TIME_SCALE_DOWN, Key::PAGE_DOWN);
		s_source->BindKey(InputAction::TIME_SCALE_UP, Key::PAGE_UP);

		//s_source->BindKey( InputAction::TOGGLE_CONSOLE, Key::F2, InputMode::DEBUG );

		s_source->BindKey(InputAction::SELECT_MESH, Key::MOUSE_LEFT, InputMode::DEBUG);
		s_source->BindKey(InputAction::SHOOT, Key::MOUSE_LEFT, InputMode::GAME);
	}

	void InputSystem::SetMode(uint8 id)
	{
		InputMode& mode = InputMode::Access(id);
		m_nextMode = id;
	}

	IInputSource& InputSystem::GetSource() const
	{
		return *s_source;
	}
}
