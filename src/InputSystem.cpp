//
// InputSystem.cpp
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#include "PCH.h"
#include "InputSystem.h"

#include "IInputSource.h"
#include "InputBindings.h"

#include <imgui/imgui.h>

dd::Service<dd::IInputSource> s_source;

namespace dd
{
	InputSystem::InputSystem()
	{
	}

	void InputSystem::Update(float delta_t)
	{
		s_source->UpdateInput();

		// update input
		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			Array<InputEvent, 64> events;
			s_source->GetKeyEvents(events);
			m_bindings->Dispatch(events);
		}

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			Array<InputEvent, 64> events;
			s_source->GetMouseEvents(events);
			m_bindings->Dispatch(events);
		}
	}

	void InputSystem::BindKeys()
	{
		s_source->BindKey(InputAction::TOGGLE_DEBUG_UI, Key::ESCAPE);
		s_source->BindKey(InputAction::TOGGLE_FREECAM, Key::F1);
		s_source->BindKey(InputAction::TOGGLE_BOUNDS, Key::F2);
		s_source->BindKey(InputAction::TOGGLE_PICKING, Key::F3, InputMode::DEBUG);
		s_source->BindKey(InputAction::FORWARD, 'W');
		s_source->BindKey(InputAction::BACKWARD, 'S');
		s_source->BindKey(InputAction::LEFT, 'A');
		s_source->BindKey(InputAction::RIGHT, 'D');
		s_source->BindKey(InputAction::UP, ' ');
		s_source->BindKey(InputAction::ADD_MINOR_TRAUMA, 'R');
		s_source->BindKey(InputAction::ADD_MAJOR_TRAUMA, 'T');
		s_source->BindKey(InputAction::DOWN, Key::LCTRL);
		s_source->BindKey(InputAction::BOOST, Key::LSHIFT);
		s_source->BindKey(InputAction::TOGGLE_ENTITY_DATA, 'E', InputMode::DEBUG);
		s_source->BindKey(InputAction::CAMERA_POS_1, '1');
		s_source->BindKey(InputAction::CAMERA_POS_2, '2');
		s_source->BindKey(InputAction::CAMERA_POS_3, '3');
		s_source->BindKey(InputAction::CAMERA_POS_4, '4');
		s_source->BindKey(InputAction::DECREASE_DEPTH, Key::HOME);
		s_source->BindKey(InputAction::INCREASE_DEPTH, Key::END);
		s_source->BindKey(InputAction::RESET_PHYSICS, 'P', InputMode::GAME);
		s_source->BindKey(InputAction::TOGGLE_PICKING, 'P', InputMode::DEBUG);
		s_source->BindKey(InputAction::PAUSE, Key::PAUSE);
		s_source->BindKey(InputAction::TIME_SCALE_DOWN, Key::PAGE_DOWN);
		s_source->BindKey(InputAction::TIME_SCALE_UP, Key::PAGE_UP);

		//s_source->BindKey( InputAction::TOGGLE_CONSOLE, Key::F2, InputMode::DEBUG );

		s_source->BindMouseButton(InputAction::SELECT_MESH, MouseButton::LEFT, InputMode::DEBUG);
		s_source->BindMouseButton(InputAction::SHOOT, MouseButton::LEFT, InputMode::GAME);
	}

	IInputSource& InputSystem::GetSource() const
	{
		return *s_source;
	}
}
